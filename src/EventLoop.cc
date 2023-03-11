#include "EventLoop.h"

EventLoop::EventLoop() : m_poller(std::make_unique<Poller>()) {
}

EventLoop::~EventLoop() = default;

void EventLoop::loop() {
    while (true) {
        std::vector<Channel*> active_channels = m_poller->poll();
        for (Channel* active_channel : active_channels) {
            active_channel->handle_event();
        }
    }
}

void EventLoop::update_channel(Channel* channel) {
    m_poller->update_channel(channel);
}

void EventLoop::delete_channel(Channel* channel) {
    m_poller->delete_channel(channel);
}
