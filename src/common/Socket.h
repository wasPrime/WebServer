#pragma once

#include "InetAddress.h"

class Socket {
public:
    Socket();
    Socket(int fd);
    ~Socket();

    void bind(InetAddress* addr);
    void listen();
    void set_non_blocking();

    int accept(InetAddress* addr);
    void connect(InetAddress* addr);

    int get_fd();

private:
    int m_fd;
};
