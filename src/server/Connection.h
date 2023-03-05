#pragma once

#include <functional>

#include "EventLoop.h"
#include "common/Buffer.h"
#include "common/Socket.h"

class Connection {
public:
    Connection(EventLoop* loop, Socket* client_socket);
    ~Connection();

    void echo(int sockfd);
    void set_delete_connection_callback(std::function<void(Socket*)> callback);

private:
    EventLoop* m_loop;
    Socket* m_client_socket;
    std::unique_ptr<Channel> m_channel;
    std::function<void(Socket*)> m_delete_connection_callback;

    std::unique_ptr<Buffer> m_read_buffer;
};
