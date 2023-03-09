#include "Poller.h"

#include <unistd.h>

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
#endif
