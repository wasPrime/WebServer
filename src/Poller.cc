#include "Poller.h"

#include <unistd.h>

#include <array>
#include <cstring>

#include "util.h"

inline constexpr int MAX_EVENTS = 1000;

#if defined(OS_LINUX)

Poller::Poller() {
    m_fd = epoll_create1(0);
    errif(m_fd == -1, "epoll create error");
    m_events.resize(MAX_EVENTS);
    // no need to call `bzero` due to already initialized in `resize`
    // bzero(m_events.data(), sizeof(epoll_event) * MAX_EVENTS);
}

Poller::~Poller() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Poller::update_channel(Channel* channel) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    if (channel->get_listen_events() & Channel::READ_EVENT) {
        ev.events |= EPOLLIN | EPOLLPRI;
    }
    if (channel->get_listen_events() & Channel::WRITE_EVENT) {
        ev.events |= EPOLLOUT;
    }
    if (channel->get_listen_events() & Channel::ET) {
        ev.events |= EPOLLET;
    }

    int sockfd = channel->get_fd();
    if (!channel->get_exist()) {
        errif(epoll_ctl(m_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll add error");
        channel->set_exist();
        // debug("Poller: add Channel to epoll tree success, the Channel's fd is: ", fd);
    } else {
        errif(epoll_ctl(m_fd, EPOLL_CTL_MOD, sockfd, &ev) == -1, "epoll modify error");
        // debug("Poller: modify Channel in epoll tree success, the Channel's fd is: ", fd);
    }
}

void Poller::delete_channel(Channel* channel) {
    // Note: fd doesn't be deleted from epoll mannually as it is removed when ::close
    // int sockfd = channel->get_fd();
    // errif(epoll_ctl(m_fd, EPOLL_CTL_DEL, sockfd, nullptr) == -1, "epoll delete error");

    channel->set_exist(false);
}

std::vector<Channel*> Poller::poll(int timeout) {
    std::vector<Channel*> active_channels;
    int nfds = epoll_wait(m_fd, m_events.data(), MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");

    for (auto&& event : std::views::take(m_events, nfds)) {
        Channel* ch = static_cast<Channel*>(event.data.ptr);

        uint32_t events = event.events;
        if (events & EPOLLIN) {
            ch->set_ready_events(Channel::READ_EVENT);
        }
        if (events & EPOLLOUT) {
            ch->set_ready_events(Channel::WRITE_EVENT);
        }
        if (events & EPOLLET) {
            ch->set_ready_events(Channel::ET);
        }

        active_channels.push_back(ch);
    }

    return active_channels;
}

#elif defined(OS_MACOS)

Poller::Poller() {
    m_fd = kqueue();
    errif(m_fd == -1, "epoll create error");
    m_events.resize(MAX_EVENTS);
    // no need to call `bzero` due to already initialized in `resize`
    // bzero(m_events.data(), sizeof(epoll_event) * MAX_EVENTS);
}

Poller::~Poller() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Poller::update_channel(Channel* channel) {
    std::array<struct kevent, 2> ev;
    memset(ev.data(), 9, sizeof(ev));

    int n = 0;
    int op = EV_ADD;
    if (channel->get_listen_events() & Channel::ET) {
        op |= EV_CLEAR;
    }
    int sockfd = channel->get_fd();
    if (channel->get_listen_events() & Channel::READ_EVENT) {
        EV_SET(&ev[n++], sockfd, EVFILT_READ, op, 0, 0, channel);
    }
    if (channel->get_listen_events() & Channel::WRITE_EVENT) {
        EV_SET(&ev[n++], sockfd, EVFILT_WRITE, op, 0, 0, channel);
    }

    int res = kevent(m_fd, ev.data(), n, nullptr, 0, nullptr);
    errif(res == -1, "kqueue add event error");
}

void Poller::delete_channel(Channel* channel) {
    std::array<struct kevent, 2> ev;
    int n = 0;
    int sockfd = channel->get_fd();
    if (channel->get_listen_events() & Channel::READ_EVENT) {
        EV_SET(&ev[n++], sockfd, EVFILT_READ, EV_DELETE, 0, 0, channel);
    }
    if (channel->get_listen_events() & Channel::WRITE_EVENT) {
        EV_SET(&ev[n++], sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, channel);
    }
    int res = kevent(m_fd, ev.data(), n, nullptr, 0, nullptr);
    errif(res == -1, "kqueue delete event error");

    channel->set_exist(false);
}

std::vector<Channel*> Poller::poll(int timeout) {
    std::vector<Channel*> active_channels;
    timespec ts;
    memset(&ts, 0, sizeof(ts));
    if (timeout != -1) {
        ts.tv_sec = timeout / 1000;
        ts.tv_nsec = static_cast<long>(timeout % 1000) * 1000 * 1000;
    }

    int nfds =
        kevent(m_fd, nullptr, 0, m_events.data(), MAX_EVENTS, (timeout != -1 ? &ts : nullptr));
    for (int i = 0; i < nfds; ++i) {
        auto channel = static_cast<Channel*>(m_events[i].udata);
        int events = m_events[i].filter;
        switch (events) {
            case EVFILT_READ: {
                channel->set_ready_events(Channel::READ_EVENT | Channel::ET);
                break;
            }
            case EVFILT_WRITE: {
                channel->set_ready_events(Channel::WRITE_EVENT | Channel::ET);
                break;
            }
            default: {
                break;
            }
        }

        active_channels.push_back(channel);
    }

    return active_channels;
}

#endif
