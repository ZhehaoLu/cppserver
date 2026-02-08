#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    // AF_INET: 表示IP地址类型IPv4
    // SOCK_STREAM: 表示TCP协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 服务器地址结构体 - IP地址和端口设置
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 服务器需要绑定地址和端口，以便客户端能够连接到服务器
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 服务器开始监听，等待客户端连接
    listen(sockfd, SOMAXCONN);

    // 服务器接受客户端连接，获取客户端的地址信息
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);

    printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    return 0;
}
