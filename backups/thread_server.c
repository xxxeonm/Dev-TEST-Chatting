#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 1024
#define MAX_CLIENTSIZE 5

int clntNum = 0;

void errorHandling(char *message);
void *t_function(void *data);

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;

    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    socklen_t clntAddrSize;

    pthread_t clntSocks[MAX_CLIENTSIZE];

    if (argc != 2)
    {
        printf("Usage:%s <port>\n", argv[0]);
        exit(1);
    }

    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == -1)
        errorHandling("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) 
        errorHandling("bind() error");

    if (listen(servSock, 5) == -1) 
        errorHandling("listen() error");

    while(1) {
        clntAddrSize = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrSize); 

        if (clntSock == -1)
            errorHandling("accept() error");

        if (clntNum == MAX_CLIENTSIZE)
        {
            printf("Client Full(max: %d)\n", MAX_CLIENTSIZE);
            close(clntSock);
            continue;
        }

        if(pthread_create(&clntSocks[clntNum], NULL, t_function, (void *)&clntSock) != 0)
        {
            errorHandling("pthread_create() error");
            close(clntSock);
            continue;
        }

        printf(">> %s:%d is connecting ...\n",inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
        clntNum++;
        printf(">> Current User %d\n", clntNum);
    }
    close(servSock);
    return 0;
}

void errorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *t_function(void *arg)
{
    int clntSock = *((int *)arg);
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    while(1) {
        char recvBuf[BUFSIZE];
        memset(&recvBuf, 0, sizeof(recvBuf));
        if (read(clntSock, recvBuf, sizeof(recvBuf)) == -1)
            errorHandling("SERVER read() error");
        printf("Client: %s", recvBuf);
        
        if (strcmp(recvBuf, "exit\n") == 0) {
            printf("CLIENT disconnected\n");
            break;
        }

        char sendBuf[BUFSIZE];
        memset(&sendBuf, 0, sizeof(sendBuf));
        printf("You(Server): ");
        fgets(sendBuf, BUFSIZE, stdin);

        if (write(clntSock, sendBuf, sizeof(sendBuf)) == -1)
            errorHandling("SERVER write() error");
            
        if (strcmp(sendBuf, "exit\n") == 0) break;
    }
    clntNum--;
    printf(">> Current User %d\n", clntNum);
    close(clntSock);
}