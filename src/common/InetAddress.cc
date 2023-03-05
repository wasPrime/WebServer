
#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress() : m_addr_len(sizeof(m_addr)) {
    bzero(&m_addr, sizeof(m_addr));
}

InetAddress::InetAddress(const char* ip, uint16_t port) : m_addr_len(sizeof(m_addr)) {
    bzero(&m_addr, sizeof(m_addr));

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}

InetAddress::~InetAddress() = default;
