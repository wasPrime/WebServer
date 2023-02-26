#pragma once

#include <unordered_map>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

class Server {
public:
    Server(EventLoop* loop);
    ~Server();

    void new_connection(Socket* client_socket);
    void delete_connection(Socket* client_socket);

private:
    EventLoop* m_loop;

    std::unique_ptr<Acceptor> m_acceptor;
    std::unordered_map<int, std::unique_ptr<Connection>> m_connections;
};
