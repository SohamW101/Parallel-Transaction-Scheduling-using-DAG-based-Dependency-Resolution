#include "Executor.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

static mutex coutMutex;

// Sequential execution
void Executor::executeSequential(const DAG &dag) {
    cout << "\nExecuting transactions sequentially (topological order)...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    queue<string> q;
    for (auto &p : indeg)
        if (p.second == 0)
            q.push(p.first);

    int step = 1;
    while (!q.empty()) {
        string tx = q.front(); q.pop();
        cout << "  Step " << step++ << ": Executing " << tx << "\n";

        for (auto &nbr : adj.at(tx)) {
            indeg[nbr]--;
            if (indeg[nbr] == 0)
                q.push(nbr);
        }
    }
    cout << "Sequential execution complete.\n";
}

// Parallel batch simulation (Commit 3)
void Executor::executeParallelBatches(const DAG &dag) {
    cout << "\nExecuting transactions in PARALLEL batches (simulated)...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    vector<string> batch;
    int batchNum = 1;

    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    while (!batch.empty()) {
        cout << "\nBatch " << batchNum++ << ": ";
        for (auto &tx : batch) cout << tx << " ";
        cout << "\n";

        vector<string> nextBatch;
        for (auto &tx : batch) {
            for (auto &nbr : adj.at(tx)) {
                indeg[nbr]--;
                if (indeg[nbr] == 0)
                    nextBatch.push_back(nbr);
            }
        }

        batch = nextBatch;
    }

    cout << "\nParallel batch simulation complete.\n";
}

// Commit 4: Real multithreaded parallel execution
void Executor::executeParallelBatchesWithThreads(const DAG &dag) {
    cout << "\nExecuting transactions in PARALLEL batches (with threads)...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    vector<string> batch;
    int batchNum = 1;

    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    while (!batch.empty()) {
        cout << "\nStarting Batch " << batchNum++ << ":\n";

        vector<thread> threads;
        for (auto &tx : batch) {
            threads.emplace_back([&, tx]() {
                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "  Executing " << tx << " in thread " 
                         << this_thread::get_id() << "...\n";
                }
                this_thread::sleep_for(chrono::milliseconds(500 + rand() % 500));
                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "  Completed " << tx << "\n";
                }
            });
        }

        for (auto &t : threads)
            t.join();

        vector<string> nextBatch;
        for (auto &tx : batch) {
            for (auto &nbr : adj.at(tx)) {
                indeg[nbr]--;
                if (indeg[nbr] == 0)
                    nextBatch.push_back(nbr);
            }
        }

        batch = nextBatch;
    }

    cout << "\nAll batches executed successfully with multithreading.\n";
}