#pragma once

#include <string>

#include "macros.h"

class Buffer {
public:
    Buffer();
    ~Buffer();

    DISALLOW_COPY_AND_MOVE(Buffer);

    void set_buf(const char* buf);
    void append(const char* str, std::size_t size);

    std::size_t size() const;
    const char* c_str() const;

    void getline();
    void clear();

private:
    std::string m_buf;
};
