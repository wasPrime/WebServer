#include <iostream>

#include "Connection.h"
#include "Server.h"
#include "SignalHandler.h"

int main() {
    Server server;

    SignalHandler::register_signal_handler(SIGINT, [] {
        std::cout << "Server exit!" << std::endl;
        exit(0);
    });

    server.on_connect([](Connection* conn) {
        std::cout << "New connection fd: " << conn->get_socket()->get_fd() << std::endl;
    });

    server.on_message([](Connection* conn) {
        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": "
                  << conn->get_read_buffer() << std::endl;

        conn->send(conn->get_read_buffer());
    });

    server.start();

    return 0;
}
