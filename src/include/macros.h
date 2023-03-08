#pragma once

#include <cassert>

#define ASSERT(expr, message) assert((expr) && (message))

#define DISALLOW_COPY(clazz)      \
    clazz(const clazz&) = delete; \
    clazz& operator=(const clazz&) = delete

#define DISALLOW_MOVE(clazz) \
    clazz(clazz&&) = delete; \
    clazz& operator=(clazz&&) = delete

#define DISALLOW_COPY_AND_MOVE(clazz) \
    DISALLOW_COPY(clazz);             \
    DISALLOW_MOVE(clazz)
