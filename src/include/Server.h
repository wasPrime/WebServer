#pragma once

#include <unordered_map>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "macros.h"

class Server {
public:
    Server(EventLoop* loop);
    ~Server();

    DISALLOW_COPY_AND_MOVE(Server);

    void new_connect(std::function<void(Connection*)> callback);
    void on_message(std::function<void(Connection*)> callback);

private:
    void new_connection(Socket* client_socket);
    void delete_connection(Socket* client_socket);

private:
    EventLoop* m_main_reactor;
    std::vector<std::unique_ptr<EventLoop>> m_sub_reactors;

    std::unique_ptr<Acceptor> m_acceptor;
    std::unordered_map<int, std::unique_ptr<Connection>> m_connections;

    std::unique_ptr<ThreadPool> m_thread_pool;

    std::function<void(Connection*)> m_new_connect_callback;
    std::function<void(Connection*)> m_on_message_callback;
};