#include "Epoll.h"

#include <unistd.h>

#include <cstring>
#include <ranges>

#include "util.h"

inline constexpr int MAX_EVENTS = 1000;

Epoll::Epoll() {
    m_epoll_fd = epoll_create1(0);
    errif(m_epoll_fd == -1, "epoll create error");
    m_events.resize(MAX_EVENTS);
    // no need to call `bzero` due to already initialized in `resize`
    // bzero(m_events.data(), sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (m_epoll_fd != -1) {
        ::close(m_epoll_fd);
        m_epoll_fd = -1;
    }
}

void Epoll::add_fd(int fd, uint32_t op) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event error");
}

void Epoll::update_channel(Channel* channel) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->get_events();

    int fd = channel->get_fd();
    if (!channel->get_in_epoll()) {
        errif(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->set_in_epoll();
        // debug("Epoll: add Channel to epoll tree success, the Channel's fd is: ", fd);
    } else {
        errif(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
        // debug("Epoll: modify Channel in epoll tree success, the Channel's fd is: ", fd);
    }
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> active_channels;
    int nfds = epoll_wait(m_epoll_fd, m_events.data(), MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");

    for (auto&& event : std::views::take(m_events, nfds)) {
        Channel* ch = static_cast<Channel*>(event.data.ptr);
        ch->set_ready_events(event.events);
        active_channels.push_back(ch);
    }

    return active_channels;
}
