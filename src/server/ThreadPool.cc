#include "ThreadPool.h"

#include <ranges>

ThreadPool::ThreadPool(int size) {
    m_threads.reserve(size);

    for (int i : std::views::iota(0, size)) {
        printf("ThreadPool create thread %d\n", i);

        m_threads.push_back(std::thread([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->m_tasks_mtx);
                    this->m_cv.wait(lock,
                                    [this] { return this->m_stop || !this->m_tasks.empty(); });
                    if (this->m_stop && this->m_tasks.empty()) {
                        return;
                    }

                    task = this->m_tasks.front();
                    this->m_tasks.pop();
                }

                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        m_stop = true;
    }
    m_cv.notify_all();

    for (std::thread& t : m_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ThreadPool::add(std::function<void()> func) {
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        if (m_stop) {
            throw std::runtime_error("ThreadPoll already stop, can't add task any more");
        }
        m_tasks.push(func);
    }

    m_cv.notify_one();
}
