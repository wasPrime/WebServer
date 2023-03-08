#pragma once

#include <functional>

#include "EventLoop.h"
#include "macros.h"

class EventLoop;
class Channel {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    DISALLOW_COPY_AND_MOVE(Channel);

    int get_fd();

    void use_ET();
    void enable_read();
    void enable_write();

    int get_listen_events() const;

    int get_ready_events() const;
    void set_ready_events(int events);

    bool get_exist() const;
    void set_exist(bool exist = true);

    void set_read_callback(std::function<void()> read_callback);
    void set_write_callback(std::function<void()> write_callback);
    void handle_event();

    static inline constexpr int READ_EVENT = 1;
    static inline constexpr int WRITE_EVENT = 1 << 1;
    static inline constexpr int ET = 1 << 2;

private:
    EventLoop* m_loop;
    int m_fd;
    int m_listen_events;
    int m_ready_events;
    bool m_exist;

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
};
