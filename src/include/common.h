#pragma once

#include <cassert>

#define DISALLOW_COPY(clazz)      \
    clazz(const clazz&) = delete; \
    clazz& operator=(const clazz&) = delete

#define DISALLOW_MOVE(clazz) \
    clazz(clazz&&) = delete; \
    clazz& operator=(clazz&&) = delete

#define DISALLOW_COPY_AND_MOVE(clazz) \
    DISALLOW_COPY(clazz);             \
    DISALLOW_MOVE(clazz)

enum class ReturnCode {
    RC_UNDEFINED,
    RC_SUCCESS,
    RC_SOCKET_ERROR,
    RC_POLLER_ERROR,
    RC_CONNECTION_ERROR,
    RC_ACCEPTOR_ERROR,
    RC_UNIMPLEMENTED
};
