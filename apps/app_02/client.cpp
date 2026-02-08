#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "util.h"


int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    
    while(true){
        char buf[1024];
        bzero(&buf, sizeof(buf));
        // 从标准输入读取数据，并将数据发送给服务器
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        // write 返回正数表示成功写入了数据
        // write 返回-1表示发生了错误，可能是服务器已经关闭了连接
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // 客户端从服务器读取数据，并将数据打印到标准输出
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes > 0){
            // read 返回正数表示成功读取了数据
            printf("message from server: %s\n", buf);
        }else if(read_bytes == 0){
            // read 返回0表示服务器关闭了连接
            printf("server socket disconnected!\n");
            break;
        }else if(read_bytes == -1){
            // read 返回-1表示发生了错误
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
    return 0;
}
