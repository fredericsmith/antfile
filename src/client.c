/**
 * @ Author: frederic
 * @ Create Time: 2020-11-01 22:09:16
 * @ Modified by: frederic
 * @ Modified time: 2020-11-11 00:01:34
 * @ Description: linux 网页客户端
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

int catch_signal(int sig, void (*handler)(int))
{
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction(sig, &action, NULL);
}

// 连接服务器
int connect_to_server(char *ip)
{
    int s = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in si;
    memset(&si, 0, sizeof(si));
    si.sin_family = PF_INET;
    si.sin_addr.s_addr = inet_addr(ip);
    si.sin_port = htons(30000);
    connect(s, (struct sockaddr *)&si, sizeof(si));
    return s;
}

// 连接服务器
int connect_to_server_by_domain(char *domain, char *port)
{
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
        error("Can't open socket");
    struct addrinfo *res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(domain, port, &hints, &res);
    connect(s, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return s;
}

// 向socket发送数据
int say(int socket, char *s)
{
    int result = send(socket, s, strlen(s), 0);
    if (result == -1)
        fprintf(stderr, "%s: %s\n", "发送数据时发生了错误", strerror(errno));

    return result;
}

int s; // socket
void handle_shutdown(int sig)
{
    if (s)
        close(s);
    fprintf(stderr, "Bye!\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    if (catch_signal(SIGINT, handle_shutdown) == -1)
        error("Can’t set the interrupt handler");

    char *domain = "tencent.anyany.top";
    char *port = "30000";
    s = connect_to_server_by_domain(domain, port);

    FILE *saveFile = fopen("receive_data.txt", "wb+");
    char recBuf[9];
    int totalReceive = 0;
    int bytesRcvd = recv(s, recBuf, 8, 0);
    while (bytesRcvd)
    {
        if (bytesRcvd == -1)
            error("Can't read from server"); // 退出
        if (bytesRcvd > 0)
        {
            totalReceive += bytesRcvd;
            recBuf[bytesRcvd] = '\0';
            printf("received: % 4d byte: %s\n", totalReceive, recBuf);
        }
        fwrite(recBuf, 1, bytesRcvd, saveFile);
        bytesRcvd = recv(s, recBuf, 8, 0);
    }
    
    printf("total received: %4d bytees data\n", totalReceive);
    fclose(saveFile);
    close(s);

    return 0;
}
