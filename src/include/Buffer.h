#pragma once

#include <string>

class Buffer {
public:
    Buffer();
    ~Buffer();

    void append(const char* str, std::size_t size);
    std::size_t size();
    const char* c_str();
    void clear();
    void getline();

private:
    std::string m_buf;
};
