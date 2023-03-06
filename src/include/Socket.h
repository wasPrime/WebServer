#pragma once

#include "InetAddress.h"

class Socket {
public:
    Socket();
    Socket(int fd);
    ~Socket();

    void bind(InetAddress* addr) const;
    void listen() const;

    int accept(InetAddress* addr) const;
    void connect(InetAddress* addr) const;
    void connect(const char* ip, uint16_t port) const;

    int get_fd() const;

    void set_non_blocking() const;
    bool is_non_blocking() const;

private:
    int m_fd;
};
