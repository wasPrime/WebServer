#include "Acceptor.h"

#include "utils/util.h"

Acceptor::Acceptor(EventLoop* loop) : m_loop(loop), m_server_socket(std::make_unique<Socket>()) {
    InetAddress server_addr(LOCAL_HOST, PORT);
    m_server_socket->bind(&server_addr);
    m_server_socket->listen();
    m_server_socket->set_non_blocking();

    m_accept_channel = std::make_unique<Channel>(loop, m_server_socket->get_fd());
    std::function<void()> cb = [this] { this->accept_connection(); };
    m_accept_channel->set_callback(cb);
    m_accept_channel->enable_reading();
}

Acceptor::~Acceptor() = default;

void Acceptor::accept_connection() {
    m_new_connection_callback(m_server_socket.get());
}

void Acceptor::set_new_connection_callback(std::function<void(Socket*)> callback) {
    m_new_connection_callback = callback;
}
