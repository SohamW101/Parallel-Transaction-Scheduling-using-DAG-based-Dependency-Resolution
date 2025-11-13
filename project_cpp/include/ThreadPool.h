#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
using namespace std;

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex queueMutex;
    condition_variable condition;
    bool stop;

public:
    ThreadPool(size_t threads);
    void enqueue(function<void()> task);
    void waitAll();
    ~ThreadPool();
};

#endif // THREAD_POOL_H