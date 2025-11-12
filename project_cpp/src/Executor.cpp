#include "Executor.h"
#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

// Sequential Topological Execution (from Commit 2)
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
        string tx = q.front();
        q.pop();
        cout << "  Step " << step++ << ": Executing " << tx << "\n";

        for (auto &nbr : adj.at(tx)) {
            indeg[nbr]--;
            if (indeg[nbr] == 0)
                q.push(nbr);
        }
    }

    cout << "Sequential execution complete.\n";
}

// Parallel Batch Execution (Commit 3)
void Executor::executeParallelBatches(const DAG &dag) {
    cout << "\nExecuting transactions in PARALLEL batches...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    vector<string> batch;
    int batchNum = 1;

    // Find all nodes with indegree 0 initially
    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    while (!batch.empty()) {
        cout << "\nBatch " << batchNum++ << " (executed in parallel): ";
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

    cout << "\nParallel batch execution complete.\n";
}