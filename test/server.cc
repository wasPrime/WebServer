#include "Server.h"

#include "EventLoop.h"

int main() {
    EventLoop loop;
    Server server(&loop);
    loop.loop();

    return 0;
}
