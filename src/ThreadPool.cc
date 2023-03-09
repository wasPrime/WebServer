#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int size) {
    m_threads.reserve(size);

    for (unsigned int i = 0; i < size; ++i) {
        printf("ThreadPool create thread %d\n", i);

        m_threads.emplace_back([this] {
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
        });
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
