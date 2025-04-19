// backend/src/ThreadPool.cpp
#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads)
    : stop_(false)
{
    for (size_t i = 0; i < numThreads; ++i) {
        // Создаём поток и запускаем в нём workerLoop
        workers_.emplace_back([this]() { this->workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    // Сигналим всем потокам остановиться
    stop_.store(true);
    condition_.notify_all();

    // Ждём завершения работы каждого потока
    for (std::thread &worker : workers_) {
        if (worker.joinable())
            worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        // Блокируем очередь и добавляем задачу
        std::unique_lock<std::mutex> lock(queueMutex_);
        tasks_.push(std::move(task));
    }
    // Сообщаем одному потоку, что задача появилась
    condition_.notify_one();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
        {
            // Ждём, пока в очереди появится задача или придёт сигнал остановиться
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this]() {
                return stop_.load() || !tasks_.empty();
            });

            // Если нужно остановиться и задач нет — выходим
            if (stop_.load() && tasks_.empty())
                return;

            // Достаём задачу из очереди
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        // Выполняем задачу вне блокировки
        task();
    }
}
