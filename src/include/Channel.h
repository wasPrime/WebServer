#pragma once

#include <functional>

#include "EventLoop.h"

class EventLoop;
class Channel {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int get_fd();

    void use_ET();
    void enable_reading();

    uint32_t get_events();

    uint32_t get_ready_events();
    void set_ready_events(uint32_t ready_events);

    bool get_in_epoll();
    void set_in_epoll();

    void set_read_callback(std::function<void()> read_callback);
    void handle_event();

private:
    EventLoop* m_loop;
    int m_fd;
    uint32_t m_events;
    uint32_t m_ready_events;
    bool m_in_epoll;

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
};
