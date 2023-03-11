#pragma once

#include <cstdint>

#include "common.h"

class Socket {
public:
    Socket();
    ~Socket();

    DISALLOW_COPY_AND_MOVE(Socket);

    void set_fd(int fd);
    [[nodiscard]] int get_fd() const;

    ReturnCode create();

    ReturnCode bind(const char* ip, uint16_t port) const;
    [[nodiscard]] ReturnCode listen() const;
    ReturnCode accept(int& client_fd) const;

    ReturnCode connect(const char* ip, uint16_t port) const;

    [[nodiscard]] ReturnCode set_non_blocking() const;
    [[nodiscard]] bool is_non_blocking() const;

private:
    int m_fd{-1};
};
