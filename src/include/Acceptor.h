#pragma once

#include <functional>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "macros.h"

class Acceptor {
public:
    Acceptor(EventLoop* loop);
    ~Acceptor();

    DISALLOW_COPY_AND_MOVE(Acceptor);

    void accept_connection();
    void set_new_connection_callback(std::function<void(Socket*)> callback);

private:
    EventLoop* m_loop;
    std::unique_ptr<Socket> m_server_socket;
    std::unique_ptr<Channel> m_accept_channel;
    std::function<void(Socket*)> m_new_connection_callback;
};
