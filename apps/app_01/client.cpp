#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 客户端地址结构体 - IP地址和端口设置
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 客户端通过connect连接服务器
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));    
    
    return 0;
}
