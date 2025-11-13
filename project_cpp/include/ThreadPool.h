#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
using namespace std;

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex queueMutex;
    condition_variable condition;
    bool stop;

    atomic<size_t> activeTasks;

public:
    ThreadPool(size_t threads);
    void enqueue(function<void()> task);
    void waitAll(); // waits until both queue empty and active tasks 0
    ~ThreadPool();
};

#endif // THREAD_POOL_H