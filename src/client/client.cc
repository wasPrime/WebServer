#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstdio>
#include <cstring>

#include "utils/util.h"

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    server_address.sin_port = htons(PORT);

    errif(
        connect(sockfd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) == -1,
        "socket connect error");

    while (true) {
        std::array<char, 1024> buf;
        bzero(&buf, sizeof(buf));
        scanf("%s", buf.data());
        ssize_t write_bytes = write(sockfd, buf.data(), sizeof(buf));
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf.data(), sizeof(buf));
        if (read_bytes > 0) {
            printf("message from server: %s\n", buf.data());
        } else if (read_bytes == 0) {
            printf("server socket disconnected!\n");
            break;
        } else if (read_bytes == -1) {
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);

    return 0;
}
