/**
 * @ Author: frederic
 * @ Create Time: 2020-11-01 21:49:59
 * @ Modified by: frederic
 * @ Modified time: 2020-11-10 23:53:03
 * @ Description: 第一个网络程序 
 *      linux 网络服务器, 只发送消息
 *      测试命令: telnet tencent.anyany.top 30000
 */

#include <stdio.h> 
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/**
 * @brief print error message
 * 
 * @param msg 
 */
void error(char *msg)
{
    fprintf(stderr, "error: %s", msg);
    exit(1);
}

/**
 * @brief antfile
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    int listener_d = socket(PF_INET, SOCK_STREAM, 0);
    if (listener_d == -1)
        error("无法打开套接字");
    struct sockaddr_in name;
    name.sin_family = PF_INET;
    name.sin_port = (in_port_t)htons(30000);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    int c = bind(listener_d, (struct sockaddr *)&name, sizeof(name));
    if (c == -1)
        error("无法绑定端口");
    if (listen(listener_d, 10) == -1)
        error("无法监听");
    puts("ant file \nWaiting for connection");
    int reuse = 1;
    if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
        error("无法设置套接字的“重新使用端口”选项");
        
    FILE *fromFile;
    char sendBuf[8];
    while (1)
    {
        struct sockaddr_storage client_addr;
        unsigned int address_size = sizeof(client_addr);
        int connect_d = accept(listener_d, (struct sockaddr *)&client_addr, &address_size);
        if (connect_d == -1)
            error("无法打开副套接字");

        // 打开文件
        fromFile = fopen("data.txt", "rb");
        
        
        int hasReadSize = 0;
        int readSize = 0;
        while(readSize = fread(sendBuf, 1, 8, fromFile)) {
            
            hasReadSize += readSize;
            printf("read % 4d bytes from file, total % 4d bytes\n", readSize, hasReadSize);
            if (send(connect_d, sendBuf, readSize, 0) == -1)
                error("sent"); 
        }
        fclose(fromFile);
        
        printf("sent: %4d bytees data\n", hasReadSize);
        close(connect_d);
    }
    return 0;
}