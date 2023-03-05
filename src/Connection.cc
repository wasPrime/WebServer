#include "Connection.h"

#include <unistd.h>

#include <cstring>

#include "util.h"

inline constexpr int READ_BUFFER = 1024;

Connection::Connection(EventLoop* loop, Socket* client_socket)
    : m_loop(loop),
      m_client_socket(client_socket),
      m_channel(std::make_unique<Channel>(loop, client_socket->get_fd())),
      m_read_buffer(std::make_unique<Buffer>()) {
    std::function<void()> callback = [this] { this->echo(this->m_client_socket->get_fd()); };
    m_channel->set_read_callback(callback);
    m_channel->enable_reading();
    m_channel->use_ET();
    m_channel->set_use_thread_pool(true);
}

Connection::~Connection() {
    delete m_client_socket;
}

void Connection::echo(int sockfd) {
    std::array<char, READ_BUFFER> buf;

    while (true) {
        bzero(&buf, sizeof(buf));

        ssize_t read_bytes = ::read(sockfd, buf.data(), sizeof(buf));
        if (read_bytes > 0) {
            m_read_buffer->append(buf.data(), read_bytes);
        } else if (read_bytes == -1 &&
                   errno == EINTR) {  // client interrupts normally and continue to read
            printf("continue reading");
            continue;
        } else if (read_bytes == -1 &&
                   ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // indicate that all data is
                                                                     // read due to non-block IO

            printf("finish reading once, errno: %d\n", errno);

            printf("message from client fd %d: %s\n", sockfd, m_read_buffer->c_str());
            errif(::write(sockfd, m_read_buffer->c_str(), m_read_buffer->size()) == -1,
                  "socket write error");
            m_read_buffer->clear();

            break;
        } else if (read_bytes == 0) {  // EOF, client disconnected
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);  // fd will be removed from the epoll tree when close socket
            m_delete_connection_callback(sockfd);
            break;
        } else {
            printf("Connection reset by peer\n");
            m_delete_connection_callback(sockfd);
            break;
        }
    }
}

void Connection::set_delete_connection_callback(std::function<void(int)> callback) {
    m_delete_connection_callback = callback;
}
