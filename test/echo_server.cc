#include <iostream>

#include "Connection.h"
#include "Server.h"
#include "util.h"

int main() {
    Server server(SERVER_IP, SERVER_PORT);

    server.on_connect([](Connection* conn) {
        std::cout << "New connection fd: " << conn->get_socket()->get_fd() << std::endl;
    });

    server.on_message([](Connection* conn) {
        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": " << conn->get_read_buffer()
                  << std::endl;

        conn->send(conn->get_read_buffer());
    });

    server.start();

    return 0;
}
