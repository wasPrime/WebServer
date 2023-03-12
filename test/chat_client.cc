#include <iostream>
#include <thread>

#include "Connection.h"
#include "Socket.h"
#include "util.h"

int main() {
    Socket socket;
    socket.create();
    socket.connect(SERVER_IP, SERVER_PORT);

    Connection conn(socket.get_fd(), nullptr);

    // TODO: Currently can't quit automatically.
    std::thread getline_thread([&conn] {
        ReturnCode send_ret = ReturnCode::RC_SUCCESS;
        while (send_ret == ReturnCode::RC_SUCCESS) {
            std::string input;
            std::getline(std::cin, input);

            send_ret = conn.send(input.c_str());
        }
    });

    std::thread read_thread([&conn] {
        while (true) {
            ReturnCode read_ret = conn.read();
            if (read_ret != ReturnCode::RC_SUCCESS) {
                std::cout << "read failed, ret: " << int(read_ret) << std::endl;
                break;
            }

            std::cout << "Message from server: " << conn.get_read_buffer() << std::endl;
        }
    });

    getline_thread.join();
    read_thread.join();

    return 0;
}
