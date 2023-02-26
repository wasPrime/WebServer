#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>
#include <ranges>

#include "utils/util.h"
// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

inline constexpr int MAX_EVENTS = 1024;
inline constexpr int READ_BUFFER = 1024;

void setnonblocking(int fd) { fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); }

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    server_address.sin_port = htons(PORT);

    errif(bind(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == -1,
          "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    int epollfd = epoll_create1(0);
    errif(epollfd == -1, "epoll create error");

    std::array<struct epoll_event, MAX_EVENTS> events;
    bzero(&events, sizeof(events));

    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

    while (true) {
        int nfds = epoll_wait(epollfd, events.data(), MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");
        for (auto&& event : std::views::take(events, nfds)) {
            if (event.data.fd == sockfd) {
                struct sockaddr_in client_address;
                bzero(&client_address, sizeof(client_address));
                socklen_t clnt_addr_len = sizeof(client_address);

                int client_sockfd =
                    accept(sockfd, reinterpret_cast<sockaddr*>(&client_address), &clnt_addr_len);
                errif(client_sockfd == -1, "socket accept error");
                // std::cout << std::format("new client fd {}! IP: {} Port: {}", client_sockfd,
                //                          inet_ntoa(client_address.sin_addr),
                //                          ntohs(client_address.sin_port))
                //           << std::endl;
                printf("new client fd %d! IP: %s Port: %d\n", client_sockfd,
                       inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(client_sockfd);
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev);
            } else if (event.events & EPOLLIN) {
                std::array<char, READ_BUFFER> buf;
                while (true) {
                    bzero(&buf, sizeof(buf));

                    int client_sockfd = event.data.fd;
                    ssize_t read_bytes = read(client_sockfd, buf.data(), sizeof(buf));
                    if (read_bytes > 0) {
                        printf("message from client fd %d: %s\n", client_sockfd, buf.data());
                        write(client_sockfd, buf.data(), sizeof(buf));
                    } else if (read_bytes == -1 &&
                               errno == EINTR) {  // client interrupts normally and continue to read
                        printf("continue reading");
                        continue;
                    } else if (read_bytes == -1 &&
                               ((errno == EAGAIN) ||
                                (errno == EWOULDBLOCK))) {  // indicate that all data is read due to
                                                            // non-block IO
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if (read_bytes == 0) {  // EOF, client disconnected
                        printf("EOF, client fd %d disconnected\n", client_sockfd);
                        close(client_sockfd);  // fd will be removed from the epoll tree when close
                                               // socket
                        break;
                    }
                }
            } else {  // other events
                printf("something else happened\n");
            }
        }
    }

    close(sockfd);

    return 0;
}
