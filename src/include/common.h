#pragma once

#include <cassert>

#define DISALLOW_COPY(clazz)                                               \
    /* NOLINT(bugprone-macro-parentheses) */ clazz(const clazz&) = delete; \
    /* NOLINT(bugprone-macro-parentheses) */ clazz& operator=(const clazz&) = delete

#define DISALLOW_MOVE(clazz)                                          \
    /* NOLINT(bugprone-macro-parentheses) */ clazz(clazz&&) = delete; \
    /* NOLINT(bugprone-macro-parentheses) */ clazz& operator=(clazz&&) = delete

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
