#include "Connection.h"

#include <unistd.h>

#include <cstring>

#include "macros.h"
#include "util.h"

inline constexpr int READ_BUFFER = 1024;

Connection::Connection(EventLoop* loop, Socket* socket)
    : m_loop(loop),
      m_socket(socket),
      m_state(State::Connected),
      m_read_buffer(std::make_unique<Buffer>()),
      m_send_buffer(std::make_unique<Buffer>()) {
    if (m_loop != nullptr) {
        m_channel = std::make_unique<Channel>(loop, socket->get_fd());
        m_channel->enable_read();
        m_channel->use_ET();
    }
}

Connection::~Connection() {
    delete m_socket;
}

void Connection::set_delete_connection_callback(std::function<void(Socket*)> callback) {
    m_delete_connection_callback = callback;
}

void Connection::set_on_message_callback(const std::function<void(Connection*)>& callback) {
    m_on_message_callback = callback;
    m_channel->set_read_callback([this] { this->business(); });
}

void Connection::business() {
    read();
    m_on_message_callback(this);
}

void Connection::read() {
    ASSERT(m_state == State::Connected, "connection state is disconnected!");

    m_read_buffer->clear();

    if (m_socket->is_non_blocking()) {
        read_non_blocking();
    } else {
        read_blocking();
    }
}

void Connection::read_non_blocking() {
    int sockfd = m_socket->get_fd();
    std::array<char, 1024> buf;

    while (true) {
        memset(&buf, 0, sizeof(buf));

        ssize_t read_bytes = ::read(sockfd, buf.data(), buf.size());
        if (read_bytes > 0) {
            m_read_buffer->append(buf.data(), read_bytes);
        } else if (read_bytes == -1 && errno == EINTR) {  // normal program interrupt
            printf("continue reading\n");
            continue;
        } else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        } else if (read_bytes == 0) {  // EOF, client disconnect
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = State::Closed;
            close();
            break;
        } else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            close();
            break;
        }
    }
}

void Connection::read_blocking() {
    int sockfd = m_socket->get_fd();
    unsigned int receive_size = 0;
    socklen_t len = sizeof(receive_size);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &receive_size, &len);

    std::vector<char> buf(receive_size);
    ssize_t read_bytes = ::read(sockfd, buf.data(), receive_size * sizeof(char));
    if (read_bytes > 0) {
        m_read_buffer->append(buf.data(), read_bytes);
    } else if (read_bytes == 0) {
        printf("read EOF, blocking client fd %d disconnected\n", sockfd);
        m_state = State::Closed;
    } else if (read_bytes == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
}

void Connection::write() {
    ASSERT(m_state == State::Connected, "connection state is disconnected!");

    if (m_socket->is_non_blocking()) {
        write_non_blocking();
    } else {
        write_blocking();
    }

    m_send_buffer->clear();
}

void Connection::send(const char* msg) {
    set_send_buffer(msg);
    write();
}

void Connection::write_non_blocking() {
    int sockfd = m_socket->get_fd();

    const char* buf = m_send_buffer->c_str();
    int data_size = m_send_buffer->size();

    int data_left = data_size;
    while (data_left > 0) {
        ssize_t write_bytes = ::write(sockfd, buf + data_size - data_left, data_left);
        if (write_bytes == -1 && errno == EINTR) {
            printf("continue writing\n");
            continue;
        }
        if (write_bytes == -1 && errno == EAGAIN) {
            break;
        }
        if (write_bytes == -1) {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            break;
        }

        data_left -= write_bytes;
    }
}

void Connection::write_blocking() {
    int sockfd = m_socket->get_fd();
    ssize_t bytes_write = ::write(sockfd, m_send_buffer->c_str(), m_send_buffer->size());
    if (bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
}

const char* Connection::get_read_buffer() const {
    return m_read_buffer->c_str();
}

void Connection::set_read_buffer(const char* str) {
    m_read_buffer->set_buf(str);
}

const char* Connection::get_send_buffer() const {
    return m_send_buffer->c_str();
}

void Connection::set_send_buffer(const char* str) {
    m_send_buffer->set_buf(str);
}

void Connection::getline_send_buffer() {
    m_send_buffer->getline();
}

Connection::State Connection::get_state() const {
    return m_state;
}

Socket* Connection::get_socket() const {
    return m_socket;
}

void Connection::close() {
    m_delete_connection_callback(m_socket);
}
