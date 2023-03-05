#include <unistd.h>

#include <array>
#include <cstring>

#include "common/Buffer.h"
#include "common/Socket.h"
#include "utils/util.h"

int main() {
    InetAddress server_addr(LOCAL_HOST, PORT);
    Socket socket;
    socket.connect(&server_addr);

    Buffer send_buffer;
    Buffer read_buffer;

    while (true) {
        send_buffer.getline();
        ssize_t write_bytes = ::write(socket.get_fd(), send_buffer.c_str(), send_buffer.size());
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        ssize_t already_read_bytes = 0;
        std::array<char, 1024> buf;
        while (true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = ::read(socket.get_fd(), buf.data(), buf.size());
            if (read_bytes > 0) {
                read_buffer.append(buf.data(), read_bytes);
                already_read_bytes += read_bytes;
            } else if (read_bytes == 0) {
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }

            if (already_read_bytes >= ssize_t(send_buffer.size())) {
                printf("message from server: %s\n", send_buffer.c_str());
                break;
            }
        }

        read_buffer.clear();
    }

    return 0;
}
