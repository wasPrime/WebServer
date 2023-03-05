#pragma once

#include "ThreadPool.h"

template <typename Callable, typename... Args>
auto ThreadPool::add(Callable&& f, Args&&... args)
    -> std::future<typename std::result_of<Callable(Args...)>::type> {
    using return_type = typename std::result_of<Callable(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Callable>(f), std::forward<Args>(args)...));

    std::future<return_type> future = task->get_future();
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);

        if (m_stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        m_tasks.push([task] { (*task)(); });
    }

    m_cv.notify_one();

    return future;
}
