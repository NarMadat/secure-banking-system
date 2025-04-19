
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

    
    void enqueue(std::function<void()> task);

private:
    
    std::vector<std::thread> workers_;
    
    std::queue<std::function<void()>> tasks_;

    
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;  

    
    void workerLoop();
};
