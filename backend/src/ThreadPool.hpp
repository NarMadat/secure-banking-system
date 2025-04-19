// backend/src/ThreadPool.hpp
#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Добавляем задачу в очередь
    void enqueue(std::function<void()> task);

private:
    // Рабочие потоки
    std::vector<std::thread> workers_;
    // Очередь задач
    std::queue<std::function<void()>> tasks_;

    // Синхронизация
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;  // флаг остановки

    // Функция цикла каждого потока
    void workerLoop();
};
