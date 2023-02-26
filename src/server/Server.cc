#include "Server.h"

#include "utils/util.h"

Server::Server(EventLoop* loop) : m_loop(loop), m_acceptor(std::make_unique<Acceptor>(loop)) {
    std::function<void(Socket*)> callback = [this](Socket* client_socket) {
        this->new_connection(client_socket);
    };
    m_acceptor->set_new_connection_callback(callback);
}

Server::~Server() = default;

void Server::new_connection(Socket* client_socket) {
    auto connection = std::make_unique<Connection>(m_loop, client_socket);
    std::function<void(Socket*)> callback = [this](Socket* client_sock) {
        this->delete_connection(client_sock);
    };
    connection->set_delete_connection_callback(callback);
    m_connections[client_socket->get_fd()] = std::move(connection);
}

void Server::delete_connection(Socket* client_socket) {
    m_connections.erase(client_socket->get_fd());
}
