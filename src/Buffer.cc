#include "Buffer.h"

#include <iostream>
#include <ranges>

Buffer::Buffer() = default;

Buffer::~Buffer() = default;

void Buffer::append(const char* str, std::size_t size) {
    m_buf.append(str, size);
}

std::size_t Buffer::size() {
    return m_buf.size();
}

const char* Buffer::c_str() {
    return m_buf.c_str();
}

void Buffer::clear() {
    m_buf.clear();
}

void Buffer::getline() {
    m_buf.clear();
    std::getline(std::cin, m_buf);
}
