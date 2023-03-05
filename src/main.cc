#include "server/EventLoop.h"
#include "server/Server.h"

int main() {
    EventLoop loop;
    Server server(&loop);
    loop.loop();

    return 0;
}
