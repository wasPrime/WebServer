#include <iostream>

#include "ThreadPool.h"

void print(int a, double b, const char* c, std::string d) {
    std::cout << a << " " << b << " " << c << " " << d << std::endl;
}

void test() {
    std::cout << "test" << std::endl;
}

int main() {
    int size = std::thread::hardware_concurrency();
    ThreadPool thread_pool(size);

    std::function<void()> func = std::bind(print, 1, 3.14, "hello", std::string("world"));
    thread_pool.add(func);

    func = test;
    thread_pool.add(func);

    return 0;
}
