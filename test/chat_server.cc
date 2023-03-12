#include <iostream>
#include <string>
#include <unordered_map>

#include "Server.h"
#include "util.h"

int main() {
    std::unordered_map<int, Connection*> clients;

    Server server(SERVER_IP, SERVER_PORT);

    server.on_connect([&](Connection* conn) {
        int client_fd = conn->get_socket()->get_fd();
        std::cout << "New connection fd: " << client_fd << std::endl;

        std::string log_in_msg;
        log_in_msg.append("Client[").append(std::to_string(client_fd)).append("] logs in");
        for (auto& each : clients) {
            Connection* another_client = each.second;
            another_client->send(log_in_msg.c_str());
        }

        clients[client_fd] = conn;
    });

    server.on_message([&](Connection* conn) {
        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": " << conn->get_read_buffer()
                  << std::endl;

        std::string broadcast_msg;
        broadcast_msg.append("Client[")
            .append(std::to_string(conn->get_socket()->get_fd()))
            .append("] said: ")
            .append(conn->get_read_buffer());
        for (auto& [fd_in_map, client_in_map] : clients) {
            if (fd_in_map == conn->get_socket()->get_fd()) {
                continue;
            }

            client_in_map->send(broadcast_msg.c_str());
        }
    });

    server.start();

    return 0;
}
