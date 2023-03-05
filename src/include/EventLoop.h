#pragma once

#include <memory>

#include "Epoll.h"

class Epoll;
class Channel;
class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void update_channel(Channel* channel);

private:
    std::unique_ptr<Epoll> m_epoll;
    bool m_quit;
};
