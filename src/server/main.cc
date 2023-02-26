#include "EventLoop.h"
#include "Server.h"

int main() {
    EventLoop loop;
    Server server(&loop);
    loop.loop();

    return 0;
}
