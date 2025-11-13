#include "ThreadPool.h"
#include <iostream>
using namespace std;

ThreadPool::ThreadPool(size_t threads) : stop(false) {
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
                }

                task();
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
    while (true) {
        {
            lock_guard<mutex> lock(queueMutex);
            if (tasks.empty())
                break;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
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