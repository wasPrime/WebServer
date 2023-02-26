#pragma once

#include "EventLoop.h"
#include "Socket.h"

class Server {
public:
    Server(EventLoop* loop);
    ~Server();

    void handle_read_event(int sockfd);
    void new_connection(Socket* server_socket);

private:
    EventLoop* m_loop;
};
