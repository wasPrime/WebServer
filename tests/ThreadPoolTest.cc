#include <iostream>

#include "server/ThreadPool.h"

void print(int a, double b, const char* c, std::string d) {
    std::cout << a << " " << b << " " << c << " " << d << std::endl;
}

void test() {
    std::cout << "test" << std::endl;
}

int main() {
    ThreadPool thread_pool;

    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    thread_pool.add(func);

    func = test;
    thread_pool.add(func);

    return 0;
}
