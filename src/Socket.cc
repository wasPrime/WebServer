#include "Socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

#include "common.h"

Socket::Socket() : m_fd(-1) {
}

Socket::~Socket() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

void Socket::set_fd(int fd) {
    m_fd = fd;
}

int Socket::get_fd() const {
    return m_fd;
}

ReturnCode Socket::create() {
    assert(m_fd == -1);  // make sure it hasn't been inititialized yet

    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) {
        perror("Failed to create socket");
        return ReturnCode::RC_SOCKET_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Socket::bind(const char* ip, uint16_t port) const {
    assert(m_fd != -1);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    if (::bind(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == -1) {
        perror("Failed to bind socket");
        return ReturnCode::RC_SOCKET_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Socket::listen() const {
    assert(m_fd != -1);

    if (::listen(m_fd, SOMAXCONN) == -1) {
        perror("Failed to listen socket");
        return ReturnCode::RC_SOCKET_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Socket::accept(int& client_fd) const {
    auto accept_with_non_blocking = [&]() -> ReturnCode {
        while (true) {
            client_fd = ::accept(m_fd, nullptr, nullptr);
            if (client_fd != -1) {
                return ReturnCode::RC_SUCCESS;
            }
            // else: client_fd == -1
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }

            perror("Failed to accept socket");
            return ReturnCode::RC_SOCKET_ERROR;
        }
    };

    auto accept_with_blocking = [&]() -> ReturnCode {
        client_fd = ::accept(m_fd, nullptr, nullptr);
        if (client_fd == -1) {
            perror("Failed to accept socket");
            return ReturnCode::RC_SOCKET_ERROR;
        }

        return ReturnCode::RC_SUCCESS;
    };

    return is_non_blocking() ? accept_with_non_blocking() : accept_with_blocking();
}

ReturnCode Socket::connect(const char* ip, uint16_t port) const {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    auto connect_with_non_blocking = [&]() -> ReturnCode {
        // TODO: The recommended practice for connecting non-blocking socket is below:
        //
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

        while (true) {
            int ret = ::connect(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
            if (ret == 0) {
                return ReturnCode::RC_SUCCESS;
            }
            if (ret == -1 && errno != EINPROGRESS) {
                perror("Failed to connect socket");
                return ReturnCode::RC_SOCKET_ERROR;
            }
        }
    };

    auto connect_with_blocking = [&]() -> ReturnCode {
        int ret = ::connect(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
        if (ret == -1) {
            perror("Failed to connect socket");
            return ReturnCode::RC_SOCKET_ERROR;
        }

        return ReturnCode::RC_SUCCESS;
    };

    return is_non_blocking() ? connect_with_non_blocking() : connect_with_blocking();
}

ReturnCode Socket::set_non_blocking() const {
    if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK) == -1) {
        perror("Socket set non-blocking failed");
        return ReturnCode::RC_SOCKET_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
}

bool Socket::is_non_blocking() const {
    return (fcntl(m_fd, F_GETFL) & O_NONBLOCK) != 0;
}
