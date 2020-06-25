#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024

void errorHandling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in servAddr;
    char message[30];
    int strLen;


    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0); 
    if (sock == -1)
        errorHandling("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) 
        errorHandling("connect() error");

    printf(">> Connected in %s:%d\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

    while(1) {
        char sendBuf[BUFSIZE];
        memset(sendBuf, 0, sizeof(sendBuf));
        printf("You(Client): ");
        fgets(sendBuf, BUFSIZE, stdin);

        if (write(sock, sendBuf, sizeof(sendBuf)) == -1)
            errorHandling("CLIENT write() error");
            
        if (strcmp(sendBuf, "exit\n") == 0) break;

        char recvBuf[BUFSIZE];
        memset(recvBuf, 0, sizeof(recvBuf));
        if (read(sock, recvBuf, sizeof(recvBuf)) == -1)
            errorHandling("CLIENT read() error");
        printf("Server: %s", recvBuf);

        if (strcmp(recvBuf, "exit\n") == 0) {
            printf("SERVER disconnected\n");
            break;
        }
    }
    
    close(socket);
    return 0;
}

void errorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
