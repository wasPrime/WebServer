#include <arpa/inet.h>
#include <sys/socket.h>

#include <cassert>
#include <cstring>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);

    assert(!connect(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)));

    return 0;
}
