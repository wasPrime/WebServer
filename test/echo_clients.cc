#include <unistd.h>

#include <iostream>
#include <mutex>

#include "Connection.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

class ExecuteGroup {
public:
    explicit ExecuteGroup(int thread_count, int msg_count) : m_thread_count(thread_count), m_msg_count(msg_count) {
    }

    void run() {
        ThreadPool thread_pool(m_thread_count);
        for (int i = 0; i < m_thread_count; ++i) {
            {
                std::lock_guard<std::mutex> lock(m_print_mtx);
                std::cout << "add event[" << i << "] to thread pool" << std::endl;
            }

            thread_pool.add([this, i] {
                single_client(i, m_msg_count);
            });
        }
    }

private:
    void single_client(int client_id, int msg_count) {
        Socket socket;
        socket.create();
        socket.connect(SERVER_IP, SERVER_PORT);

        Connection conn(socket.get_fd(), nullptr);

        for (int i = 0; i < msg_count; ++i) {
            conn.send("I'm a client");
            if (conn.get_state() == Connection::State::Closed) {
                conn.close();
                break;
            }

            conn.read();
            {
                std::lock_guard<std::mutex> lock(m_print_mtx);
                std::cout << "[client_id: " << client_id << "][msg_id: " << i << "] msg: " << conn.get_read_buffer()
                          << std::endl;
            }
        }
    }

private:
    int m_thread_count;
    int m_msg_count;

    std::mutex m_print_mtx;
};

int main() {
    int client_id = 10;
    int msg_count = 10;

    ExecuteGroup execute_group(client_id, msg_count);
    execute_group.run();

    return 0;
}
