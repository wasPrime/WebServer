#include "Server.h"

#include <ranges>

#include "utils/util.h"

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
    std::function<void(int)> callback = [this](int client_sockfd) {
        this->delete_connection(client_sockfd);
    };
    connection->set_delete_connection_callback(callback);
    m_connections[client_socket->get_fd()] = std::move(connection);
}

void Server::delete_connection(int client_sockfd) {
    if (client_sockfd == -1) {
        return;
    }

    m_connections.erase(client_sockfd);
}
