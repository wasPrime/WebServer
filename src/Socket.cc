#include "Socket.h"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>

#include "util.h"

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

void Socket::bind(InetAddress* addr) const {
    errif(::bind(m_fd, reinterpret_cast<const sockaddr*>(&addr->get_addr()),
                 sizeof(addr->get_addr())) == -1,
          "socket bind error");
}

void Socket::listen() const {
    errif(::listen(m_fd, SOMAXCONN) == -1, "socket listen error");
}

int Socket::accept(InetAddress* addr) const {
    int client_sockfd = -1;
    sockaddr_in tmp_addr;
    socklen_t addr_len = sizeof(tmp_addr);

    if (is_non_blocking()) {
        while (true) {
            client_sockfd = ::accept(m_fd, reinterpret_cast<sockaddr*>(&tmp_addr), &addr_len);
            if (client_sockfd == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // printf("no connection yet\n");
                    continue;
                } else {
                    errif(true, "socket accept error");
                }
            } else {
                break;
            }
        }
    } else {
        client_sockfd = ::accept(m_fd, reinterpret_cast<sockaddr*>(&tmp_addr), &addr_len);
        errif(client_sockfd == -1, "socket accept error");
    }

    addr->set_addr(tmp_addr);

    return client_sockfd;
}

void Socket::connect(InetAddress* addr) const {
    // for client socket
    if (fcntl(m_fd, F_GETFL) & O_NONBLOCK) {
        while (true) {
            int ret = ::connect(m_fd, reinterpret_cast<const sockaddr*>(&addr->get_addr()),
                                sizeof(addr->get_addr()));
            if (ret == 0) {
                break;
            }
            if (ret == -1) {
                if (errno == EINPROGRESS) {
                    continue;

                    // 连接非阻塞式sockfd建议的做法：
                    // The recommended practice for connecting non-blocking socket is below:
                    // The socket is nonblocking and the connection cannot be
                    // completed immediately.  (UNIX domain sockets failed with
                    // EAGAIN instead.)  It is possible to select(2) or poll(2)
                    // for completion by selecting the socket for writing.  After
                    // select(2) indicates writability, use getsockopt(2) to read
                    // the SO_ERROR option at level SOL_SOCKET to determine
                    // whether connect() completed successfully (SO_ERROR is
                    // zero) or unsuccessfully (SO_ERROR is one of the usual
                    // error codes listed here, explaining the reason for the
                    // failure).
                    //
                    // For simplicity here consistently try to connect until succeed
                    // and it's equivalent to blocking

                } else {
                    errif(true, "socket connect error");
                }
            }
        }
    } else {
        errif(::connect(m_fd, reinterpret_cast<const sockaddr*>(&addr->get_addr()),
                        sizeof(addr->get_addr())) == -1,
              "socket connect error");
    }
}

void Socket::connect(const char* ip, uint16_t port) const {
    InetAddress server_addr(ip, port);
    connect(&server_addr);
}

int Socket::get_fd() const {
    return m_fd;
}

void Socket::set_non_blocking() const {
    fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK);
}

bool Socket::is_non_blocking() const {
    return (fcntl(m_fd, F_GETFL) & O_NONBLOCK) != 0;
}
