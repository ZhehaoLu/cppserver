#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll
{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    void updateChannel(Channel*);

    // 如果一个服务器同时提供不同的服务，如HTTP、FTP等，那么就算文件描述符上发生的事件都是可读事件
    // 不同的连接类型也将决定不同的处理逻辑，仅仅通过一个文件描述符来区分显然会很麻烦
    // 我们更加希望拿到关于这个文件描述符更多的信息
    std::vector<Channel*> poll(int timeout = -1);
};

