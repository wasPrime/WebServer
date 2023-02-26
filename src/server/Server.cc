#include "Server.h"

#include <unistd.h>

#include <cstring>

#include "utils/util.h"

// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

inline constexpr int READ_BUFFER = 1024;

Server::Server(EventLoop* loop) : m_loop(loop) {
    InetAddress server_addr(LOCAL_HOST, PORT);
    auto server_socket = new Socket;
    server_socket->bind(&server_addr);
    server_socket->listen();
    server_socket->set_non_blocking();

    auto server_channel = new Channel(loop, server_socket->get_fd());
    std::function<void()> cb = [this, server_socket] { this->new_connection(server_socket); };
    server_channel->set_callback(cb);
    server_channel->enable_reading();
}

Server::~Server() = default;

void Server::handle_read_event(int sockfd) {
    std::array<char, READ_BUFFER> buf;

    while (true) {
        bzero(&buf, sizeof(buf));

        ssize_t read_bytes = read(sockfd, buf.data(), sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf.data());
            write(sockfd, buf.data(), sizeof(buf));
        } else if (read_bytes == -1 &&
                   errno == EINTR) {  // client interrupts normally and continue to read
            printf("continue reading");
            continue;
        } else if (read_bytes == -1 &&
                   ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // indicate that all data is
                                                                     // read due to non-block IO
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (read_bytes == 0) {  // EOF, client disconnected
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);  // fd will be removed from the epoll tree when close socket
            break;
        }
    }
}

void Server::new_connection(Socket* server_socket) {
    InetAddress client_addr;
    auto client_socket =
        new Socket(server_socket->accept(&client_addr));  // TODO: here is a memory leak!
    // std::cout << std::format("new client fd {}! IP: {} Port: {}",
    // client_socket->get_fd(),
    //                          inet_ntoa(client_addr.m_addr.sin_addr),
    //                          ntohs(client_addr.m_addr.sin_port))
    //           << std::endl;
    printf("new client fd %d! IP: %s Port: %d\n", client_socket->get_fd(),
           inet_ntoa(client_addr.m_addr.sin_addr), ntohs(client_addr.m_addr.sin_port));

    client_socket->set_non_blocking();

    auto client_channel = new Channel(m_loop, client_socket->get_fd());
    std::function<void()> cb = [this, client_channel] {
        this->handle_read_event(client_channel->get_fd());
    };
    client_channel->set_callback(cb);
    client_channel->enable_reading();
}
