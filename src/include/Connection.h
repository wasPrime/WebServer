#pragma once

#include <functional>

#include "Buffer.h"
#include "EventLoop.h"
#include "Socket.h"
#include "common.h"

class Connection {
public:
    enum class State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };

    Connection(int fd, EventLoop* loop);
    ~Connection();

    DISALLOW_COPY_AND_MOVE(Connection);

    void set_delete_connection_callback(const std::function<void(int)>& callback);
    void set_on_message_callback(const std::function<void(Connection*)>& callback);

    ReturnCode read();
    ReturnCode write();
    ReturnCode send(const char* msg);

    [[nodiscard]] const char* get_read_buffer() const;
    void set_read_buffer(const char* str);

    [[nodiscard]] const char* get_send_buffer() const;
    void set_send_buffer(const char* str);

    [[nodiscard]] State get_state() const;
    [[nodiscard]] Socket* get_socket() const;

    void close();

private:
    ReturnCode read_non_blocking();
    ReturnCode read_blocking();

    ReturnCode write_non_blocking();
    ReturnCode write_blocking();

    void business();

private:
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Channel> m_channel;

    State m_state;
    std::unique_ptr<Buffer> m_read_buffer;
    std::unique_ptr<Buffer> m_send_buffer;

    std::function<void(int)> m_delete_connection_callback;

    std::function<void(Connection*)> m_on_message_callback;
};
