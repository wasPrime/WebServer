#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>

#include "utils/util.h"
// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);

    errif(bind(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == -1,
          "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    struct sockaddr_in client_address;
    socklen_t clnt_addr_len = sizeof(client_address);
    bzero(&client_address, sizeof(client_address));

    int client_sockfd =
        accept(sockfd, reinterpret_cast<sockaddr*>(&client_address), &clnt_addr_len);
    errif(client_sockfd == -1, "socket accept error");

    // std::cout << std::format("new client fd {}! IP: {} Port: {}", client_sockfd,
    //                          inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port))
    //           << std::endl;
    printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_address.sin_addr),
           ntohs(client_address.sin_port));

    while (true) {
        std::array<char, 1024> buf;
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(client_sockfd, buf.data(), sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client fd %d: %s\n", client_sockfd, buf.data());
            write(client_sockfd, buf.data(), sizeof(buf));
        } else if (read_bytes == 0) {
            printf("client fd %d disconnected\n", client_sockfd);
            close(client_sockfd);
            break;
        } else if (read_bytes == -1) {
            close(client_sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);

    return 0;
}
