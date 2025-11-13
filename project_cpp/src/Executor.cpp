#include "Executor.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

mutex coutMutex;

// Commit 6: ThreadPool-based execution
void Executor::executeWithThreadPool(const DAG &dag, const vector<Transaction> &txs) {
    cout << "\nExecuting transactions with THREAD POOL parallelism...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    unordered_map<string, Transaction> lookup;
    for (const auto &tx : txs)
        lookup[tx.getId()] = tx;

    vector<string> batch;
    int batchNum = 1;

    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    ThreadPool pool(4);  // Example: fixed pool of 4 worker threads

    while (!batch.empty()) {
        cout << "\nBatch " << batchNum++ << " started:\n";

        vector<string> nextReady;

        for (auto &txID : batch) {
            pool.enqueue([&, txID]() {
                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "  Executing " << txID << " using worker thread "
                         << this_thread::get_id() << "\n";
                }

                // Simulate work
                this_thread::sleep_for(chrono::milliseconds(300));
            });
        }

        pool.waitAll();

        // Compute next batch
        for (auto &txID : batch) {
            for (auto &nbr : adj.at(txID)) {
                indeg[nbr]--;
                if (indeg[nbr] == 0)
                    nextReady.push_back(nbr);
            }
        }

        batch = nextReady;
    }

    cout << "\nThread pool execution completed.\n";
}