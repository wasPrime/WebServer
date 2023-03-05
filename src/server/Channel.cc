#include "Channel.h"

Channel::Channel(EventLoop* loop, int fd)
    : m_loop(loop),
      m_fd(fd),
      m_events(0),
      m_ready_events(0),
      m_in_epoll(false),
      m_use_thread_pool(true) {
}

Channel::~Channel() = default;

void Channel::use_ET() {
    m_events |= EPOLLET;
    m_loop->update_channel(this);
}

void Channel::enable_reading() {
    m_events |= EPOLLIN | EPOLLPRI;
    m_loop->update_channel(this);
}

int Channel::get_fd() {
    return m_fd;
}

uint32_t Channel::get_events() {
    return m_events;
}

bool Channel::get_in_epoll() {
    return m_in_epoll;
}

void Channel::set_in_epoll() {
    m_in_epoll = true;
}

uint32_t Channel::get_ready_events() {
    return m_ready_events;
}

void Channel::set_ready_events(uint32_t ready_events) {
    m_ready_events = ready_events;
}

void Channel::set_read_callback(std::function<void()> read_callback) {
    m_read_callback = read_callback;
}

void Channel::handle_event() {
    if (m_ready_events & (EPOLLIN | EPOLLPRI)) {
        if (m_use_thread_pool) {
            m_loop->add_thread(m_read_callback);
        } else {
            m_read_callback();
        }
    }

    if (m_ready_events & EPOLLOUT) {
        if (m_use_thread_pool) {
            m_loop->add_thread(m_write_callback);
        } else {
            m_write_callback();
        }
    }
}

void Channel::set_use_thread_pool(bool use) {
    m_use_thread_pool = use;
}
