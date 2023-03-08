#include <unistd.h>

#include <array>
#include <cstring>
#include <iostream>

#include "Buffer.h"
#include "Connection.h"
#include "Socket.h"
#include "util.h"

int main() {
    Socket socket;
    socket.connect(LOCAL_HOST, PORT);

    Connection conn(nullptr, &socket);

    while (true) {
        conn.getline_send_buffer();
        conn.write();
        if (conn.get_state() == Connection::State::Closed) {
            conn.close();
            break;
        }

        conn.read();
        std::cout << "Message from server: " << conn.get_read_buffer() << std::endl;
    }

    return 0;
}
