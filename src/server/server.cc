#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>
#include <ranges>

#include "Epoll.h"
#include "Socket.h"
#include "utils/util.h"
// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

inline constexpr int MAX_EVENTS = 1024;
inline constexpr int READ_BUFFER = 1024;

void handle_read_event(int sockfd);

int main() {
    InetAddress server_addr(LOCAL_HOST, PORT);
    Socket server_socket;
    server_socket.bind(&server_addr);
    server_socket.listen();
    server_socket.set_non_blocking();

    Epoll epoll;
    epoll.add_fd(server_socket.get_fd(), EPOLLIN | EPOLLET);

    while (true) {
        std::vector<epoll_event> events = epoll.poll();
        for (const epoll_event& event : events) {
            if (event.data.fd == server_socket.get_fd()) {
                InetAddress client_addr;
                auto client_socket =
                    new Socket(server_socket.accept(&client_addr));  // TODO: here is a memory leak!
                // std::cout << std::format("new client fd {}! IP: {} Port: {}",
                // client_socket.get_fd(),
                //                          inet_ntoa(client_addr.m_addr.sin_addr),
                //                          ntohs(client_addr.m_addr.sin_port))
                //           << std::endl;
                printf("new client fd %d! IP: %s Port: %d\n", client_socket->get_fd(),
                       inet_ntoa(client_addr.m_addr.sin_addr), ntohs(client_addr.m_addr.sin_port));

                client_socket->set_non_blocking();
                epoll.add_fd(client_socket->get_fd(), EPOLLIN | EPOLLET);

            } else if (event.events & EPOLLIN) {
                handle_read_event(event.data.fd);
            } else {  // other eventss
                printf("something else happened\n");
            }
        }
    }

    return 0;
}

void handle_read_event(int sockfd) {
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
