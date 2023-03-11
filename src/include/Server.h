#pragma once

#include <unordered_map>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "common.h"

class Server {
public:
    Server();
    ~Server();

    DISALLOW_COPY_AND_MOVE(Server);

    void start();

    void on_connect(const std::function<void(Connection*)>& callback);
    void on_message(const std::function<void(Connection*)>& callback);

private:
    void new_connection(int fd);
    void delete_connection(int fd);

private:
    std::unique_ptr<EventLoop> m_main_reactor;
    std::unique_ptr<Acceptor> m_acceptor;

    std::vector<std::unique_ptr<EventLoop>> m_sub_reactors;
    std::unordered_map<int, std::unique_ptr<Connection>> m_connections;

    std::unique_ptr<ThreadPool> m_thread_pool;

    std::function<void(Connection*)> m_on_connect_callback;
    std::function<void(Connection*)> m_on_message_callback;
};
