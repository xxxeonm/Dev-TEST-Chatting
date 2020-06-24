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
    int servSock;
    int clntSock;

    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    socklen_t clntAddrSize;

    char message[] = "Hello World!"; 

    if (argc != 2)
    {
        printf("Usage:%s <port>\n", argv[0]);
        exit(1);
    }

    servSock = socket(PF_INET, SOCK_STREAM, 0); //socket 함수호출을 통해서 소켓을 생성하고 있다
    if (servSock == -1)
        errorHandling("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) //bind 함수호출을 통해서 IP주소와 PORT번호를 할당하고 있다
        errorHandling("bind() error");

    if (listen(servSock, 5) == -1) //listen 함수를 호출하고 있다. 이로써 소켓은 연결요청을 받아들일 수 있는 상태가 된다
        errorHandling("listen() error");

    clntAddrSize = sizeof(clntAddr);
    clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrSize); //연결요청의 수락을 위한 accept 함수를 호출하고 있다
    //연결요청이 없는 상태에서 이 함수가 호출되면, 연결요청이 있을 때까지 함수는 반환하지 않는다

    if (clntSock == -1)
        errorHandling("accept() error");

    write(clntSock, message, sizeof(message)); //write 함수는 데이터를 전송하는 기능의 함수인데, 이 문장이 실행됬다는 것은 연결요청이 있었다는 뜻
    close(clntSock);
    close(servSock);
    return 0;
}

void errorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
