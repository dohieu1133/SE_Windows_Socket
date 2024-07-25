#include "peek2peek.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "SendData") == 0 && argc == 3)
        {
            Client client(argv[2]);
            if (client.Connect() == 1)
            {
                return 1;
            }
            printf("Is conneting %s\n", argv[2]);

            char type[20] = "";
            while (strcmp(type, "End") != 0)
            {
                scanf("%s", &type);
                if (strcmp(type, "SendText") == 0)
                {
                    client.SendText();
                }
                else if (strcmp(type, "SendFile") == 0)
                {
                    // Read <path> in SendFile <path> <buffer-size>
                    char fileName[DEFAULT_BUFLEN] = "";
                    int buffer_size = 0;

                    char c;
                    do
                    {
                        scanf("%c", &c);
                    } while (c == ' ');

                    if (c != '\"' && c != '\'')
                    {
                        fileName[0] = c;
                        fileName[1] = '\0';
                        char temp[DEFAULT_BUFLEN];
                        scanf("%s", &temp);
                        strcat(fileName, temp);
                    }
                    else
                    {
                        char temp = c;
                        int fileNameLen = 0;
                        while (true)
                        {
                            scanf("%c", &c);
                            if (c != temp)
                            {
                                fileName[fileNameLen++] = c;
                            }
                            else
                            {
                                break;
                            }
                        }
                        fileName[fileNameLen] = '\0';
                    }

                    // Read <buffer_size> in SendFile <path> <buffer-size>
                    scanf("%d", &buffer_size);
                    printf("%s-%d\n", fileName, buffer_size);
                    fflush(stdin);
                    client.SendFile(fileName, buffer_size);
                }
                else if (strcmp(type, "End") != 0)
                {
                    printf("The term \"%s is not recognized\", please try \"SendText\", \"SendFile\" or \"End\"\n", type);
                }
            }
            client.Disconnect();
        }
        else if (strcmp(argv[1], "ReceiveData") == 0 && argc >= 2)
        {
            char location_store_file[100] = ".\\";
            if (argc >= 4 && strcmp(argv[2], "-out") == 0)
            {
                strcpy(location_store_file, argv[3]);
            }
            Server server(location_store_file);

            if (server.CreateSock() == 1)
            {
                return 1;
            }
            if (server.Bind() == 1)
            {
                return 1;
            }
            if (server.Listen() == 1)
            {
                return 1;
            }
            if (server.Accept() == 1)
            {
                return 1;
            }
            printf("Is conneting\n");
            if (server.Receive() == 1)
            {
                return 1;
            }
            if (server.Disconnect() == 1)
            {
                return 1;
            }
        }
    }
}
