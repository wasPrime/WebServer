#pragma once

#include <memory>

#include "Epoll.h"
#include "ThreadPool.h"

class Epoll;
class Channel;
class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void update_channel(Channel* channel);

    void add_thread(std::function<void()> func);

private:
    std::unique_ptr<Epoll> m_epoll;
    std::unique_ptr<ThreadPool> m_thread_pool;
    bool m_quit;
};
