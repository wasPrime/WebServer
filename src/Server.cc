#include "Server.h"

#include <ranges>

#include "util.h"

Server::Server(EventLoop* loop)
    : m_main_reactor(loop),
      m_acceptor(std::make_unique<Acceptor>(
          m_main_reactor))  // Notice the init order of m_main_reactor and m_acceptor
{
    std::function<void(Socket*)> callback = [this](Socket* client_socket) {
        this->new_connection(client_socket);
    };
    m_acceptor->set_new_connection_callback(callback);

    int size = std::thread::hardware_concurrency();

    m_sub_reactors.reserve(size);
    for (int i = 0; i < size; ++i) {
        m_sub_reactors.push_back(std::make_unique<EventLoop>());
    }

    m_thread_pool = std::make_unique<ThreadPool>(size);
    for (auto i : std::views::iota(0, size)) {
        std::function<void()> sub_loop = [this, i] { this->m_sub_reactors[i]->loop(); };
        m_thread_pool->add(sub_loop);
    }
}

Server::~Server() = default;

void Server::new_connection(Socket* client_socket) {
    if (client_socket->get_fd() == -1) {
        return;
    }

    auto random = client_socket->get_fd() % m_sub_reactors.size();
    auto connection = std::make_unique<Connection>(m_sub_reactors[random].get(), client_socket);
    std::function<void(Socket*)> callback = [this](Socket* client_sock) {
        this->delete_connection(client_sock);
    };
    connection->set_delete_connection_callback(callback);
    connection->set_on_connect_callback(m_on_connect_callback);
    m_connections[client_socket->get_fd()] = std::move(connection);
}

void Server::delete_connection(Socket* client_socket) {
    m_connections.erase(client_socket->get_fd());
}

void Server::on_connect(std::function<void(Connection*)> callback) {
    m_on_connect_callback = callback;
}
