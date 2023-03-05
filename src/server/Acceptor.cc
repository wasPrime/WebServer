#include "Acceptor.h"

#include "utils/util.h"

// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

Acceptor::Acceptor(EventLoop* loop) : m_loop(loop), m_server_socket(std::make_unique<Socket>()) {
    InetAddress server_addr(LOCAL_HOST, PORT);
    m_server_socket->bind(&server_addr);
    m_server_socket->listen();

    // Set Acceptor as blocking
    // m_server_socket->set_non_blocking();

    m_accept_channel = std::make_unique<Channel>(loop, m_server_socket->get_fd());
    std::function<void()> cb = [this] { this->accept_connection(); };
    m_accept_channel->set_read_callback(cb);
    m_accept_channel->enable_reading();
    m_accept_channel->set_use_thread_pool(false);
}

Acceptor::~Acceptor() = default;

void Acceptor::accept_connection() {
    InetAddress client_addr;
    auto client_socket =
        new Socket(m_server_socket->accept(&client_addr));  // TODO: here is a memory leak!
    // std::cout << std::format("new client fd {}! IP: {} Port: {}",
    // client_socket->get_fd(),
    //                          inet_ntoa(client_addr.m_addr.sin_addr),
    //                          ntohs(client_addr.m_addr.sin_port))
    //           << std::endl;
    printf("new client fd %d! IP: %s Port: %d\n", client_socket->get_fd(),
           inet_ntoa(client_addr.m_addr.sin_addr), ntohs(client_addr.m_addr.sin_port));

    client_socket->set_non_blocking();

    m_new_connection_callback(client_socket);
}

void Acceptor::set_new_connection_callback(std::function<void(Socket*)> callback) {
    m_new_connection_callback = callback;
}
