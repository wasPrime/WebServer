#pragma once

#include <arpa/inet.h>

#include "macros.h"

class InetAddress {
public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

    DISALLOW_COPY_AND_MOVE(InetAddress);

    void set_addr(const sockaddr_in& addr);
    const sockaddr_in& get_addr() const;

    const char* get_ip() const;
    uint16_t get_port() const;

private:
    sockaddr_in m_addr;
};
