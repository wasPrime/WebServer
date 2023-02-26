#include "Epoll.h"

#include <unistd.h>

#include <cstring>
#include <ranges>

#include "utils/util.h"

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

std::vector<epoll_event> Epoll::poll(int timeout) {
    int nfds = epoll_wait(m_epoll_fd, m_events.data(), MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    return std::vector<epoll_event>(m_events.begin(), m_events.begin() + nfds);
}
