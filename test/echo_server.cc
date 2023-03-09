#include <iostream>

#include "EventLoop.h"
#include "Server.h"
#include "SignalHandler.h"

int main() {
    EventLoop loop;
    Server server(&loop);

    SignalHandler::register_signal_handler(SIGINT, [] {
        std::cout << "Server exit!" << std::endl;
        exit(0);
    });

    server.on_message([](Connection* conn) {
        if (conn->get_state() != Connection::State::Connected) {
            return;
        }

        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": "
                  << conn->get_read_buffer() << std::endl;
        conn->send(conn->get_read_buffer());
    });

    loop.loop();

    return 0;
}
