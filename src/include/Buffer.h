#pragma once

#include <string>

class Buffer {
public:
    Buffer();
    ~Buffer();

    void set_buf(const char* buf);
    void append(const char* str, std::size_t size);

    std::size_t size() const;
    const char* c_str() const;

    void getline();
    void clear();

private:
    std::string m_buf;
};
