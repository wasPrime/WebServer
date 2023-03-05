#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    void add(std::function<void()> func);

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_tasks_mtx;
    std::condition_variable m_cv;
    bool m_stop;
};
