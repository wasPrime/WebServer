#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "macros.h"

class ThreadPool {
public:
    ThreadPool(unsigned int size = 10);
    ~ThreadPool();

    DISALLOW_COPY_AND_MOVE(ThreadPool);

    template <typename Callable, typename... Args>
    auto add(Callable&& f, Args&&... args) -> std::future<std::invoke_result_t<Callable, Args...>>;

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_tasks_mtx;
    std::condition_variable m_cv;
    bool m_stop;
};

#include "ThreadPool.hpp"
