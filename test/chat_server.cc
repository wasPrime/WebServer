#include "Server.h"

#include <iostream>
#include <unordered_map>

#include "EventLoop.h"

int main() {
    std::unordered_map<int, Connection*> clients;

    EventLoop loop;
    Server server(&loop);

    server.new_connect([&](Connection* conn) {
        int client_fd = conn->get_socket()->get_fd();
        std::cout << "New connection fd: " << client_fd << std::endl;

        clients[client_fd] = conn;

        for (auto& each : clients) {
            Connection* another_client = each.second;
            another_client->send(conn->get_read_buffer());
        }
    });

    server.on_message([&](Connection* conn) {
        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": "
                  << conn->get_read_buffer() << std::endl;

        for (auto& each : clients) {
            Connection* another_client = each.second;
            another_client->send(conn->get_read_buffer());
        }
    });

    loop.loop();

    return 0;
}
