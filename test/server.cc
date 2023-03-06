#include "Server.h"

#include <iostream>

#include "EventLoop.h"

int main() {
    EventLoop loop;
    Server server(&loop);
    server.on_connect([](Connection* conn) {
        conn->read();

        if (conn->get_state() == Connection::State::Closed) {
            conn->close();
            return;
        }

        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": "
                  << conn->get_read_buffer() << std::endl;

        conn->set_send_buffer(conn->get_read_buffer());
        conn->write();
    });

    loop.loop();

    return 0;
}
