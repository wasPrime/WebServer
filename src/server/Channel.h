#pragma once

#include <sys/epoll.h>

#include "Epoll.h"

class Epoll;
class Channel {
public:
    Channel(Epoll* epoll, int fd);
    ~Channel();

    int get_fd();

    void enable_reading();

    uint32_t get_events();
    uint32_t get_revents();

    void set_revents(uint32_t revents);

    bool get_in_epoll();
    void set_in_epoll();

private:
    Epoll* m_epoll;
    int m_fd;
    uint32_t m_events;
    uint32_t m_revents;
    bool m_in_epoll;
};
