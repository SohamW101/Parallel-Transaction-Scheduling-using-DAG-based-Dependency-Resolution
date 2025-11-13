#include "ThreadPool.h"
#include <iostream>
#include <chrono>
using namespace std;

ThreadPool::ThreadPool(size_t threads) : stop(false), activeTasks(0) {
    for (size_t i = 0; i < threads; i++) {
        workers.emplace_back([this]() {
            while (true) {
                function<void()> task;

                {
                    unique_lock<mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty())
                        return;

                    task = move(tasks.front());
                    tasks.pop();
                    activeTasks.fetch_add(1, memory_order_relaxed);
                }

                // execute outside lock
                try {
                    task();
                } catch (...) {
                    // swallow exceptions for demo
                }

                activeTasks.fetch_sub(1, memory_order_relaxed);
                // after finishing a task, notify any waiters
                condition.notify_all();
            }
        });
    }
}

void ThreadPool::enqueue(function<void()> task) {
    {
        lock_guard<mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::waitAll() {
    unique_lock<mutex> lock(queueMutex);
    condition.wait(lock, [this]() {
        return tasks.empty() && activeTasks.load(memory_order_relaxed) == 0;
    });
}

ThreadPool::~ThreadPool() {
    {
        lock_guard<mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (thread &t : workers) {
        if (t.joinable())
            t.join();
    }
}