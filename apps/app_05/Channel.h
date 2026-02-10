#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel
{
private:
    Epoll *ep; // 这个Channel所属的Epoll对象
    int fd; // 这个Channel负责的文件描述符
    uint32_t events; // 这个Channel负责监听的事件
    uint32_t revents; // 这个Channel上发生的事件，由epoll返回
    bool inEpoll; // 这个Channel是否已经在epoll树上了
public:
    Channel(Epoll *_ep, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    // void setEvents(uint32_t);
    void setRevents(uint32_t);
};

