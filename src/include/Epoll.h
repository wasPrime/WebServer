#pragma once

#include <sys/epoll.h>

#include <cstdint>
#include <vector>

#include "Channel.h"

class Channel;
class Epoll {
public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    void update_channel(Channel* channel);
    std::vector<Channel*> poll(int timeout = -1);

private:
    int m_epoll_fd;
    std::vector<epoll_event> m_events;
};
