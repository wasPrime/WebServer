#include <unistd.h>

#include <iostream>

#include "Connection.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

void single_client(int msg_count, unsigned int wait) {
    auto socket = new Socket();
    socket->connect(LOCAL_HOST, PORT);
    Connection conn(nullptr, socket);

    sleep(wait);

    for (int i = 0; i < msg_count; ++i) {
        conn.set_send_buffer("I'm a client");
        conn.write();
        if (conn.get_state() == Connection::State::Closed) {
            conn.close();
            break;
        }

        conn.read();
        std::cout << "msg count " << i << ": " << conn.get_read_buffer() << std::endl;
    }
}

int main() {
    int threads = 10;
    int msgs = 10;
    int wait = 0;

    ThreadPool thread_pool(threads);
    std::function<void()> func = [&] { single_client(msgs, wait); };
    for (int i = 0; i < threads; ++i) {
        std::cout << "add event[" << i << "] to thread pool" << std::endl;

        thread_pool.add(func);
    }

    return 0;
}
