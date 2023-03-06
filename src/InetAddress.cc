
#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress() {
    bzero(&m_addr, sizeof(m_addr));
}

InetAddress::InetAddress(const char* ip, uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}

InetAddress::~InetAddress() = default;

void InetAddress::set_addr(const sockaddr_in& addr) {
    m_addr = addr;
}

const sockaddr_in& InetAddress::get_addr() const {
    return m_addr;
}

const char* InetAddress::get_ip() const {
    return inet_ntoa(m_addr.sin_addr);
}

uint16_t InetAddress::get_port() const {
    return ntohs(m_addr.sin_port);
}
