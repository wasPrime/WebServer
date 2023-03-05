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

    void set_use_thread_pool(bool use = true);

private:
    EventLoop* m_loop;
    int m_fd;
    uint32_t m_events;
    uint32_t m_ready_events;
    bool m_in_epoll;
    bool m_use_thread_pool;
    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
};
