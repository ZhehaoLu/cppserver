#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

// 服务器端使用epoll实现IO多路复用，能够同时处理多个客户端连接
// 服务器端的流程如下：
// 1. 创建一个socket，绑定地址和端口，开始监听
// 2. 创建一个epoll文件描述符，注册服务器的监听socket到epoll树上，监听可读事件和边缘触发模式
// 3. 使用epoll_wait监听epoll树上注册的文件描述符上的事件，获取发生事件的文件描述符和事件类型
// 4. 如果发生事件的文件描述符是服务器的监听socket，说明有新的客户端连接请求到来，接受客户端连接，并将该客户端的socket fd 添加到epoll树上，监听可读事件和边缘触发模式
// 5. 如果发生事件的文件描述符是某个客户端的socket fd，说明该客户端发送了数据，服务器需要读取客户端发送的数据并处理
// 6. 服务器从客户端读取数据，并将数据原样返回给客户端
// 7. 如果客户端关闭了连接，服务器需要关闭该客户端的socket fd，并将该文件描述符从epoll树上移除
// 8. 服务器继续监听epoll树上注册的文件描述符上的事件，重复步骤3-7
// 使用epoll实现IO多路复用，能够同时处理多个客户端连接，提高服务器的性能和并发能力

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    // 创建一个epoll文件描述符
    // 参数0表示默认的epoll大小，Linux 2.6.8之后这个参数被忽略了，可以设置为任意值
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    // 将要监听的文件描述符添加到epoll树上， 以后续监听这个文件描述符的事件
    // 参数1：epoll文件描述符
    // 参数2：操作类型，这里是添加
    // 参数3：要监听的文件描述符
    // 参数4：要监听的事件类型，这里是可读事件和边缘触发模式
    // 边缘触发模式：
    // 当文件描述符上有事件发生时，epoll会通知应用程序一次，之后如果不处理这个事件，
    // epoll就不会再通知应用程序了，直到应用程序处理完这个事件之后，epoll才会再次通知应用程序，
    // 这样可以减少应用程序的负载，提高性能
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    while(true){
        // 随时使用epoll_wait监听epoll树上注册的文件描述符上的事件，获取发生事件的文件描述符和事件类型
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");
        // 有nfds个事件，遍历这些事件并处理
        for(int i = 0; i < nfds; ++i){
            // 如果发生事件的文件描述符是服务器的监听socket，说明有新的客户端连接请求到来
            if(events[i].data.fd == sockfd){
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                // 将该客户端的socket fd 添加到epoll
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if(events[i].events & EPOLLIN){      //可读事件
                // 客户端发生可读事件，说明客户端发送了数据，服务器需要读取客户端发送的数据并处理
                char buf[READ_BUFFER];
                while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);   //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            } else{         //其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
    }
    close(sockfd);
    return 0;
}
