#include "peek2peek.h"

Client::Client(const char *address)
{
    strcpy(serverAddress, address);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        error = 1;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
}

int Client::Connect()
{
    // Resolve the server address and port
    iResult = getaddrinfo(serverAddress, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    return 0;
}

int Client::Send(const char *messenge, int size = -1)
{
    if (size < 0)
    {
        size = strlen(messenge);
    }
    // Send an initial buffer
    iResult = send(ConnectSocket, messenge, size, 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    iResult = recv(ConnectSocket, sendbuf, sendbuflen, 0);
    if (iResult == 0)
        printf("Connection closed\n");
    else if (iResult < 0)
        printf("recv failed with error: %d\n", WSAGetLastError());

    return 0;
}

int Client::SendText()
{
    if (Send("SendText") == 1)
    {
        return 1;
    }

    char endText[8] = "", temp[4] = "";
    while (true)
    {
        fgets(sendbuf, sendbuflen, stdin);
        if (strlen(sendbuf) <= 3)
        {
            strcpy(temp, sendbuf);
            strcat(endText, temp);
            strcpy(endText, (endText + strlen(endText) - 4));
        }
        else
        {
            strcpy(endText, (sendbuf + strlen(sendbuf) - 4));
        }
        if (Send(sendbuf) == 1)
        {
            return 1;
        }
        if (strcmp(endText, "End\n") == 0)
        {
            if (Send("End") == 1)
            {
                return 1;
            }
            break;
        }
    }
    printf("\nEnd send text\n");
    return 0;
}

int Client::SendFile(const char *filePath, int buffer_size)
{
    if (buffer_size <= 0 || buffer_size > DEFAULT_BUFLEN)
    {
        buffer_size = DEFAULT_BUFLEN;
    }
    FILE *file = fopen(filePath, "rb");
    if (file == nullptr)
    {
        printf("Can not open file %s\n", filePath);
        return 1;
    }

    std::filesystem::path pathObj(filePath);
    char fileName[DEFAULT_BUFLEN];
    strcpy(fileName, pathObj.filename().string().c_str());

    if (Send("SendFile") == 1)
    {
        fclose(file);
        return 1;
    }
    if (Send(fileName) == 1)
    {
        fclose(file);
        return 1;
    }

    char buffer[DEFAULT_BUFLEN];
    int rsize = 0;
    while (!feof(file))
    {
        rsize = fread(buffer, sizeof(char), buffer_size, file);
        if (rsize != 0)
        {
            if (Send(buffer, rsize) == 1)
            {
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);

    if (Send("End") == 1)
    {
        return 1;
    }

    printf("\nEnd send file\n");
    return 0;
}

int Client::Disconnect()
{
    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // cleanup closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}