#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <mswsock.h>
#include <filesystem>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT "27015"

class Server
{
private:
    char location_store_file[DEFAULT_BUFLEN] = ".\\";
    char filePath[DEFAULT_BUFLEN] = "";
    FILE *recvFile = nullptr;
    WSADATA wsaData;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int error = 0;
    int iResult = 0;
    int iSendResult = 0;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    void ReceiveText();
    void ReceiveFile(int);
    void CreatePath(const char *, const char *);

public:
    Server();
    Server(const char *);
    int CreateSock();
    int Bind();
    int Listen();
    int Accept();
    int Receive();
    int Disconnect();
};

class Client
{
private:
    char serverAddress[DEFAULT_BUFLEN];
    SOCKET ConnectSocket = INVALID_SOCKET;
    WSADATA wsaData;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int error = 0;
    int iResult;
    char sendbuf[DEFAULT_BUFLEN];
    int sendbuflen = DEFAULT_BUFLEN;

public:
    Client(const char *);
    int Connect();
    int Send(const char *, int);
    int SendText();
    int SendFile(const char *, int);
    int Disconnect();
};