#include "Buffer.h"

#include <iostream>

Buffer::Buffer() = default;

Buffer::~Buffer() = default;

void Buffer::set_buf(const char* buf) {
    m_buf = buf;
}

void Buffer::append(const char* str, std::size_t size) {
    m_buf.append(str, size);
}

std::size_t Buffer::size() const {
    return m_buf.size();
}

const char* Buffer::c_str() const {
    return m_buf.c_str();
}

void Buffer::getline() {
    m_buf.clear();
    std::getline(std::cin, m_buf);
}

void Buffer::clear() {
    m_buf.clear();
}
