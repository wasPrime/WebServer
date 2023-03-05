#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    template <typename Callable, typename... Args>
    auto add(Callable&& f, Args&&... args)
        -> std::future<typename std::result_of<Callable(Args...)>::type>;

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_tasks_mtx;
    std::condition_variable m_cv;
    bool m_stop;
};

#include "ThreadPool.hpp"
