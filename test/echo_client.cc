#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>

#include "Connection.h"
#include "Socket.h"
#include "util.h"

int main() {
    Socket socket;
    socket.create();
    socket.connect(LOCAL_HOST, PORT);

    Connection conn(socket.get_fd(), nullptr);

    while (true) {
        std::string input;
        std::getline(std::cin, input);
        conn.send(input.c_str());
        if (conn.get_state() == Connection::State::Closed) {
            conn.close();
            break;
        }

        conn.read();
        std::cout << "Message from server: " << conn.get_read_buffer() << std::endl;
    }

    return 0;
}
