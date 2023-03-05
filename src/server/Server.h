#pragma once

#include <unordered_map>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "common/Socket.h"

class Server {
public:
    Server(EventLoop* loop);
    ~Server();

    void new_connection(Socket* client_socket);
    void delete_connection(int client_sockfd);

private:
    EventLoop* m_main_reactor;
    std::vector<std::unique_ptr<EventLoop>> m_sub_reactors;

    std::unique_ptr<Acceptor> m_acceptor;
    std::unordered_map<int, std::unique_ptr<Connection>> m_connections;

    std::unique_ptr<ThreadPool> m_thread_pool;
};
