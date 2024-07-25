#include "peek2peek.h"
#include <windows.h>

Server::Server()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        error = 1;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        error = 1;
        return;
    }
}

Server::Server(const char *file_path)
{
    strcpy(location_store_file, file_path);
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        error = 1;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        error = 1;
        return;
    }
}

int Server::CreateSock()
{
    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    return 0;
}

int Server::Bind()
{
    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        error = 1;
    }
    freeaddrinfo(result);
    return error;
}

int Server::Listen()
{
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

int Server::Accept()
{
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    // No longer need server socket
    closesocket(ListenSocket);
    return 0;
}

void Server::ReceiveText()
{
    printf("%s", recvbuf);
}

void Server::ReceiveFile(int recvbufFilelen)
{
    fwrite(recvbuf, recvbufFilelen, 1, recvFile);
}

void Server::CreatePath(const char *cdirectory, const char *cfileName)
{

    std::filesystem::path pdirectory = cdirectory;
    std::filesystem::path pFileName = cfileName;
    std::filesystem::path fullPath = pdirectory / pFileName;

    strcpy(filePath, fullPath.string().c_str());
    FILE *file;
    errno_t err;
    int num = 0;
    char fname[DEFAULT_BUFLEN], *fextension;
    strcpy(fname, filePath);
    int j = 0;
    for (int i = strlen(fname) - 1; i >= 0; i--)
    {
        if (fname[i] == '.')
        {
            fname[i] = '\0';
            fextension = fname + i + 1;
            break;
        }
    }
    while ((err = fopen_s(&file, filePath, "r")) == 0)
    {
        num++;
        sprintf(filePath, "%s(%d).%s", fname, num, fextension);
        fclose(file);
    }
}

int Server::Receive()
{
    // Receive until the peer shuts down the connection
    int flag = 0;
    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0';
            if (flag == 0)
            {
                if (strcmp(recvbuf, "SendText") == 0)
                {
                    flag = 1;
                }
                else if (strcmp(recvbuf, "SendFile") == 0)
                {
                    flag = 2;
                }
            }
            else
            {
                if (strcmp(recvbuf, "End") == 0)
                {
                    if (flag == 3)
                    {
                        fclose(recvFile);
                    }
                    flag = 0;
                }
                if (flag == 1)
                {
                    ReceiveText();
                }
                else if (flag == 2)
                {
                    CreatePath(location_store_file, recvbuf);
                    printf("%s\n", filePath);

                    DWORD attributes = GetFileAttributesA(location_store_file);
                    if (attributes == INVALID_FILE_ATTRIBUTES)
                    {
                        // Thư mục không tồn tại, hãy tạo nó
                        if (CreateDirectoryA(location_store_file, NULL))
                        {
                            printf("Created directory %s\n", location_store_file);
                        }
                        else
                        {
                            printf("Can not created directory %s\n", location_store_file);
                            return 1;
                        }
                    }

                    recvFile = fopen(filePath, "wb");

                    if (!recvFile)
                    {
                        printf("Can not create file %s\n", filePath);
                        return 1;
                    }

                    flag = 3;
                }
                else if (flag == 3)
                {
                    ReceiveFile(iResult);
                }
            }

            // Echo the buffer back to the sender
            // Sleep(500);
            iSendResult = send(ClientSocket, "Done", recvbuflen, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                if (flag == 3)
                {
                    fclose(recvFile);
                }
                return 1;
            }
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            if (flag == 3)
            {
                fclose(recvFile);
            }
            return 1;
        }

    } while (iResult > 0);
    return 0;
}

int Server::Disconnect()
{
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}