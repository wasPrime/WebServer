#pragma once

#include <string>

#include "common.h"

class Buffer {
public:
    Buffer();
    ~Buffer();

    DISALLOW_COPY_AND_MOVE(Buffer);

    void set_buf(const char* buf);
    void append(const char* str, std::size_t size);

    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] const char* c_str() const;

    void clear();

private:
    std::string m_buf;
};
