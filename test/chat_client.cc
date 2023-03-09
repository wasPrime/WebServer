#include <iostream>

#include "Connection.h"
#include "Socket.h"
#include "util.h"

int main() {
    auto socket = new Socket();  // TODO: Adjust to grasp ownership by smart point
    socket->connect(LOCAL_HOST, PORT);

    Connection conn(nullptr, socket);

    while (true) {
        conn.read();
        std::cout << "Message from server: " << conn.get_read_buffer() << std::endl;
    }

    return 0;
}
