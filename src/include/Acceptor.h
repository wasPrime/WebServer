#pragma once

#include <functional>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

class Acceptor {
public:
    explicit Acceptor(EventLoop* loop, const char* ip, uint16_t port);
    ~Acceptor();

    DISALLOW_COPY_AND_MOVE(Acceptor);

    void accept_connection() const;
    void set_new_connection_callback(const std::function<void(int)>& callback);

private:
    std::unique_ptr<Socket> m_server_socket;
    std::unique_ptr<Channel> m_accept_channel;
    std::function<void(int)> m_new_connection_callback;
};
