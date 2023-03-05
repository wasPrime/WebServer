#include "EventLoop.h"

EventLoop::EventLoop()
    : m_epoll(std::make_unique<Epoll>()),
      m_thread_pool(std::make_unique<ThreadPool>()),
      m_quit(false) {
}

EventLoop::~EventLoop() = default;

void EventLoop::loop() {
    while (!m_quit) {
        std::vector<Channel*> active_channels = m_epoll->poll();
        for (Channel* active_channel : active_channels) {
            active_channel->handle_event();
        }
    }
}

void EventLoop::update_channel(Channel* channel) {
    m_epoll->update_channel(channel);
}

void EventLoop::add_thread(std::function<void()> func) {
    m_thread_pool->add(func);
}
