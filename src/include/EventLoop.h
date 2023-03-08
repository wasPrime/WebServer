#pragma once

#include <memory>

#include "Poller.h"
#include "macros.h"

class Poller;
class Channel;
class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    DISALLOW_COPY_AND_MOVE(EventLoop);

    void loop();
    void update_channel(Channel* channel);
    void delete_channel(Channel* channel);

private:
    std::unique_ptr<Poller> m_poller;
    bool m_quit;
};
