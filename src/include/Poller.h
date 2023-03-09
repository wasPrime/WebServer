#pragma once

#if defined(OS_LINUX)
#include <sys/epoll.h>
#elif defined(OS_MACOS)
#include <sys/event.h>
#endif

#include <cstdint>
#include <vector>

#include "Channel.h"
#include "macros.h"

class Channel;
class Poller {
public:
    Poller();
    ~Poller();

    DISALLOW_COPY_AND_MOVE(Poller);

    void update_channel(Channel* channel);
    void delete_channel(Channel* channel);

    std::vector<Channel*> poll(int timeout = -1);

private:
    int m_fd;

#if defined(OS_LINUX)
    std::vector<epoll_event> m_events;
#elif defined(OS_MACOS)
    std::vector<struct kevent> m_events;
#endif
};
