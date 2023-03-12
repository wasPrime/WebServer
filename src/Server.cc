#include "Server.h"

#include <cstddef>

Server::Server(const char* ip, uint16_t port)
    : m_main_reactor(std::make_unique<EventLoop>()),
      // Notice the init order of m_main_reactor and m_acceptor
      m_acceptor(std::make_unique<Acceptor>(m_main_reactor.get(), ip, port)),
      m_thread_pool(std::make_unique<ThreadPool>()) {
    std::function<void(int)> callback = [this](int fd) {
        this->new_connection(fd);
    };
    m_acceptor->set_new_connection_callback(callback);

    unsigned int size = std::thread::hardware_concurrency();

    m_sub_reactors.reserve(size);
    for (unsigned int i = 0; i < size; ++i) {
        m_sub_reactors.push_back(std::make_unique<EventLoop>());
    }
}

Server::~Server() = default;

void Server::start() {
    for (size_t i = 0; i < m_sub_reactors.size(); ++i) {
        std::function<void()> sub_loop = [this, i] {
            this->m_sub_reactors[i]->loop();
        };
        m_thread_pool->add(sub_loop);
    }

    m_main_reactor->loop();
}

void Server::new_connection(int fd) {
    assert(fd != -1);

    auto random = fd % m_sub_reactors.size();
    auto connection = std::make_unique<Connection>(fd, m_sub_reactors[random].get());
    std::function<void(int)> callback = [this](int client_fd) {
        this->delete_connection(client_fd);
    };
    connection->set_delete_connection_callback(callback);
    connection->set_on_message_callback(m_on_message_callback);

    if (m_on_connect_callback) {
        m_on_connect_callback(connection.get());
    }

    // Note that the ownership of the connection is transfered here
    // so m_on_connect_callback should be called in advance.
    m_connections[fd] = std::move(connection);
}

void Server::delete_connection(int fd) {
    m_connections.erase(fd);
}

void Server::on_connect(const std::function<void(Connection*)>& callback) {
    m_on_connect_callback = callback;
}

void Server::on_message(const std::function<void(Connection*)>& callback) {
    m_on_message_callback = callback;
}
