#include "Connection.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#include "common.h"

inline constexpr int READ_BUFFER = 1024;

Connection::Connection(int fd, EventLoop* loop)
    : m_socket(std::make_unique<Socket>()),
      m_read_buffer(std::make_unique<Buffer>()),
      m_send_buffer(std::make_unique<Buffer>()) {
    m_socket->set_fd(fd);

    if (loop != nullptr) {
        m_channel = std::make_unique<Channel>(fd, loop);
        m_channel->enable_read();
        m_channel->enable_ET();
    }
}

Connection::~Connection() = default;

void Connection::set_delete_connection_callback(const std::function<void(int)>& callback) {
    m_delete_connection_callback = callback;
}

void Connection::set_on_message_callback(const std::function<void(Connection*)>& callback) {
    m_on_message_callback = callback;
    m_channel->set_read_callback([this] {
        this->business();
    });
}

void Connection::business() {
    ReturnCode read_ret = read();
    if (read_ret != ReturnCode::RC_SUCCESS) {
        return;  // read_ret;
    }

    m_on_message_callback(this);
}

ReturnCode Connection::read() {
    if (m_state != State::Connected) {
        perror("Connection is not connected, can not read");
        return ReturnCode::RC_CONNECTION_ERROR;
    }

    m_read_buffer->clear();

    return m_socket->is_non_blocking() ? read_non_blocking() : read_blocking();
}

ReturnCode Connection::read_non_blocking() {
    int sockfd = m_socket->get_fd();

    std::array<char, 1024> buf{};
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
        } else if (read_bytes == 0) {  // EOF, client disconnected
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = State::Closed;
            close();
            return ReturnCode::RC_CONNECTION_ERROR;
        } else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            close();
            return ReturnCode::RC_CONNECTION_ERROR;
        }
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Connection::read_blocking() {
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
        return ReturnCode::RC_CONNECTION_ERROR;
    } else if (read_bytes == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
        return ReturnCode::RC_CONNECTION_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Connection::send(const char* msg) {
    set_send_buffer(msg);

    ReturnCode write_ret = write();
    if (write_ret != ReturnCode::RC_SUCCESS) {
        return write_ret;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Connection::write() {
    if (m_state != State::Connected) {
        perror("Connection is not connected, can not write");
        return ReturnCode::RC_CONNECTION_ERROR;
    }

    ReturnCode return_code = m_socket->is_non_blocking() ? write_non_blocking() : write_blocking();

    m_send_buffer->clear();

    return return_code;
}

ReturnCode Connection::write_non_blocking() {
    int sockfd = m_socket->get_fd();

    const char* buf = m_send_buffer->c_str();
    std::size_t total_size = m_send_buffer->size();

    std::size_t already_send_size = 0;
    while (already_send_size < total_size) {
        ssize_t write_bytes = ::write(sockfd, buf + already_send_size, total_size - already_send_size);
        if (write_bytes == -1) {
            if (errno == EINTR) {
                printf("continue writing\n");
                continue;
            }
            if (errno == EAGAIN) {
                return ReturnCode::RC_CONNECTION_ERROR;
            }

            // Other error
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            return ReturnCode::RC_CONNECTION_ERROR;
        }

        already_send_size += write_bytes;
    }

    return ReturnCode::RC_SUCCESS;
}

ReturnCode Connection::write_blocking() {
    int sockfd = m_socket->get_fd();
    // TODO:
    // Don't considering the circumstance that the size of m_send_buffer is larger than the size of
    // TCP buffer thus there may be bugs
    ssize_t bytes_write = ::write(sockfd, m_send_buffer->c_str(), m_send_buffer->size());
    if (bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
        return ReturnCode::RC_CONNECTION_ERROR;
    }

    return ReturnCode::RC_SUCCESS;
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

Connection::State Connection::get_state() const {
    return m_state;
}

Socket* Connection::get_socket() const {
    return m_socket.get();
}

void Connection::close() {
    m_delete_connection_callback(m_socket->get_fd());
}
