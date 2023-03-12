#include "Acceptor.h"

#include <sys/fcntl.h>

#include <cassert>
#include <utility>

// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

Acceptor::Acceptor(EventLoop* loop, const char* ip, uint16_t port) : m_server_socket(std::make_unique<Socket>()) {
    assert(m_server_socket->create() == ReturnCode::RC_SUCCESS);
    assert(m_server_socket->bind(ip, port) == ReturnCode::RC_SUCCESS);
    assert(m_server_socket->listen() == ReturnCode::RC_SUCCESS);

    m_accept_channel = std::make_unique<Channel>(m_server_socket->get_fd(), loop);
    std::function<void()> callback = [this] {
        this->accept_connection();
    };
    m_accept_channel->set_read_callback(callback);
    m_accept_channel->enable_read();
}

Acceptor::~Acceptor() = default;

void Acceptor::accept_connection() const {
    int client_fd = -1;
    if (m_server_socket->accept(client_fd) != ReturnCode::RC_SUCCESS) {
        perror("Acceptor failed to accept socket");
        return;  // return ReturnCode::RC_ACCEPTOR_ERROR
    }

    // set new connection as non blcoking
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK);

    if (m_new_connection_callback) {
        m_new_connection_callback(client_fd);
    }

    // return ReturnCode::RC_SUCCESS;
}

void Acceptor::set_new_connection_callback(const std::function<void(int)>& callback) {
    m_new_connection_callback = callback;
}
