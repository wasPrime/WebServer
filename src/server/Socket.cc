#include "Socket.h"

#include <fcntl.h>
#include <unistd.h>

#include "utils/util.h"

Socket::Socket() {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(m_fd == -1, "socket create error");
}

Socket::Socket(int fd) {
    m_fd = fd;
    errif(m_fd == -1, "socket create error");
}

Socket::~Socket() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Socket::bind(InetAddress* addr) {
    errif(::bind(m_fd, reinterpret_cast<sockaddr*>(&addr->m_addr), addr->m_addr_len) == -1,
          "socket bind error");
}

void Socket::listen() {
    errif(::listen(m_fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::set_non_blocking() {
    fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr) {
    int client_sockfd =
        ::accept(m_fd, reinterpret_cast<sockaddr*>(&addr->m_addr), &addr->m_addr_len);
    errif(client_sockfd == -1, "socket accept error");
    return client_sockfd;
}

int Socket::get_fd() {
    return m_fd;
}
