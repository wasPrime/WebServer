#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "common.h"

class ThreadPool {
public:
    explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency());
    ~ThreadPool();

    DISALLOW_COPY_AND_MOVE(ThreadPool);

    template <typename Callable, typename... Args>
    auto add(Callable&& f, Args&&... args) -> std::future<std::invoke_result_t<Callable, Args...>>;

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_tasks_mtx;
    std::condition_variable m_cv;
    std::atomic_bool m_stop;
};

#include "ThreadPool.hpp"
