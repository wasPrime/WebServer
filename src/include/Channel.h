#pragma once

#include <functional>

#include "EventLoop.h"
#include "common.h"

class EventLoop;
class Channel {
public:
    Channel(int fd, EventLoop* loop);
    ~Channel();

    DISALLOW_COPY_AND_MOVE(Channel);

    int get_fd();

    void enable_read();
    void enable_write();
    void enable_ET();  // NOLINT(readability-identifier-naming)

    [[nodiscard]] int get_listen_events() const;

    [[nodiscard]] int get_ready_events() const;
    void set_ready_events(int events);

    [[nodiscard]] bool get_exist() const;
    void set_exist(bool exist = true);

    void set_read_callback(const std::function<void()>& read_callback);
    void set_write_callback(const std::function<void()>& write_callback);
    void handle_event() const;

    static inline constexpr int READ_EVENT = 1;
    static inline constexpr int WRITE_EVENT = 1 << 1;
    static inline constexpr int ET = 1 << 2;

private:
    int m_fd;
    EventLoop* m_loop;
    int m_listen_events{0};
    int m_ready_events{0};
    bool m_exist{false};

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
};
