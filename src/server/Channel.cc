#include "Channel.h"

Channel::Channel(EventLoop* loop, int fd)
    : m_loop(loop), m_fd(fd), m_events(0), m_revents(0), m_in_epoll(false) {
}

Channel::~Channel() = default;

void Channel::enable_reading() {
    m_events = EPOLLIN | EPOLLET;
    m_loop->update_channel(this);
}

int Channel::get_fd() {
    return m_fd;
}

uint32_t Channel::get_events() {
    return m_events;
}

uint32_t Channel::get_revents() {
    return m_revents;
}

bool Channel::get_in_epoll() {
    return m_in_epoll;
}

void Channel::set_in_epoll() {
    m_in_epoll = true;
}

void Channel::set_revents(uint32_t revents) {
    m_revents = revents;
}

void Channel::set_callback(std::function<void()> callback) {
    m_callback = callback;
}

void Channel::handle_event() {
    m_callback();
}
