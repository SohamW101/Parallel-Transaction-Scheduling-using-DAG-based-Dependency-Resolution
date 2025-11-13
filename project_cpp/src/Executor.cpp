#include "Executor.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

extern mutex coutMutex;

// Helper comparator for priority queue
struct TxComparator {
    bool operator()(const Transaction &a, const Transaction &b) const {
        if (a.getFee() != b.getFee())
            return a.getFee() < b.getFee(); // higher fee first
        return a.getTimestamp() > b.getTimestamp(); // earlier timestamp first
    }
};

// Commit 5: Priority Scheduled Parallel Execution
void Executor::executePriorityScheduledBatches(const DAG &dag, const vector<Transaction> &txs) {
    cout << "\nExecuting transactions using PRIORITY-based parallel batches...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    // Map txID → Transaction object
    unordered_map<string, Transaction> lookup;
    for (const auto &tx : txs)
        lookup[tx.getId()] = tx;

    priority_queue<Transaction, vector<Transaction>, TxComparator> pq;

    // Insert all in-degree-0 transactions into PQ
    for (auto &p : indeg)
        if (p.second == 0)
            pq.push(lookup[p.first]);

    int batchNum = 1;

    while (!pq.empty()) {
        vector<Transaction> batch;

        // Extract all currently ready txs
        while (!pq.empty()) {
            batch.push_back(pq.top());
            pq.pop();
        }

        cout << "\nBatch " << batchNum++ << " (priority ordered): ";
        for (auto &tx : batch) cout << tx.getId() << " ";
        cout << "\n";

        vector<string> nextReady;

        // Reduce indegree of neighbors
        for (auto &tx : batch) {
            for (auto &nbr : adj.at(tx.getId())) {
                indeg[nbr]--;
                if (indeg[nbr] == 0)
                    nextReady.push_back(nbr);
            }
        }

        // Put next ready nodes into PQ by priority
        for (auto &id : nextReady)
            pq.push(lookup[id]);
    }

    cout << "\nPriority-based batch execution complete.\n";
}