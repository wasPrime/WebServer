#include <arpa/inet.h>
#include <sys/socket.h>

#include <cassert>
#include <cstring>
#include <iostream>
// TODO:
// Change the way of formatting due to the incompleteness of `std::format`
// at clang++ 15.0.6 at present
// #include <format>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);

    assert(!bind(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)));

    listen(sockfd, SOMAXCONN);

    struct sockaddr_in client_address;
    socklen_t clnt_addr_len = sizeof(client_address);
    bzero(&client_address, sizeof(client_address));

    int client_sockfd = accept(sockfd, reinterpret_cast<sockaddr*>(&client_address), &clnt_addr_len);

    // std::cout << std::format("new client fd {}! IP: {} Port: {}", client_sockfd,
    //                          inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port))
    //           << std::endl;
    printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_address.sin_addr),
           ntohs(client_address.sin_port));

    return 0;
}
