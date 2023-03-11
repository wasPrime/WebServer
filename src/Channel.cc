#include "Channel.h"

#include <utility>

Channel::Channel(int fd, EventLoop* loop) : m_fd(fd), m_loop(loop) {
}

Channel::~Channel() {
    m_loop->delete_channel(this);
}

void Channel::enable_read() {
    m_listen_events |= READ_EVENT;
    m_loop->update_channel(this);
}

void Channel::enable_write() {
    m_listen_events |= WRITE_EVENT;
    m_loop->update_channel(this);
}

void Channel::enable_ET() {
    m_listen_events |= ET;
    m_loop->update_channel(this);
}

int Channel::get_fd() {
    return m_fd;
}

int Channel::get_listen_events() const {
    return m_listen_events;
}

bool Channel::get_exist() const {
    return m_exist;
}

void Channel::set_exist(bool exist) {
    m_exist = exist;
}

int Channel::get_ready_events() const {
    return m_ready_events;
}

void Channel::set_ready_events(int events) {
    if (events & READ_EVENT) {
        m_ready_events |= READ_EVENT;
    }

    if (events & WRITE_EVENT) {
        m_ready_events |= WRITE_EVENT;
    }

    if (events & ET) {
        m_ready_events |= ET;
    }
}

void Channel::set_read_callback(const std::function<void()>& read_callback) {
    m_read_callback = read_callback;
}

void Channel::set_write_callback(const std::function<void()>& write_callback) {
    m_write_callback = write_callback;
}

void Channel::handle_event() const {
    if (m_ready_events & READ_EVENT) {
        m_read_callback();
    }

    if (m_ready_events & WRITE_EVENT) {
        m_write_callback();
    }
}
