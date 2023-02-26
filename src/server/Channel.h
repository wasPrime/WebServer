#pragma once

#include <functional>

#include "EventLoop.h"

class EventLoop;
class Channel {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int get_fd();

    void enable_reading();

    uint32_t get_events();
    uint32_t get_revents();

    void set_revents(uint32_t revents);

    bool get_in_epoll();
    void set_in_epoll();

    void set_callback(std::function<void()> callback);
    void handle_event();

private:
    EventLoop* m_loop;
    int m_fd;
    uint32_t m_events;
    uint32_t m_revents;
    bool m_in_epoll;
    std::function<void()> m_callback;
};
