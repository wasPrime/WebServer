#pragma once

#include <functional>

#include "Buffer.h"
#include "EventLoop.h"
#include "Socket.h"
#include "macros.h"

class Connection {
public:
    enum class State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
    };

    Connection(EventLoop* loop, Socket* socket);
    ~Connection();

    DISALLOW_COPY_AND_MOVE(Connection);

    void set_delete_connection_callback(std::function<void(Socket*)> callback);
    void set_on_connect_callback(const std::function<void(Connection*)>& callback);

    void read();
    void write();

    const char* get_read_buffer() const;
    void set_read_buffer(const char* str);

    const char* get_send_buffer() const;
    void set_send_buffer(const char* str);
    void getline_send_buffer();

    State get_state() const;
    Socket* get_socket() const;

    void close();

private:
    void read_non_blocking();
    void read_blocking();

    void write_non_blocking();
    void write_blocking();

private:
    EventLoop* m_loop;
    Socket* m_socket;
    std::unique_ptr<Channel> m_channel;
    State m_state{State::Invalid};
    std::unique_ptr<Buffer> m_read_buffer;
    std::unique_ptr<Buffer> m_send_buffer;

    std::function<void(Socket*)> m_delete_connection_callback;

    std::function<void(Connection*)> m_on_connect_callback;
};
