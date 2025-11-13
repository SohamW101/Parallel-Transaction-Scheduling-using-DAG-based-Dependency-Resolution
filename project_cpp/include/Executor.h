#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "DAG.h"
#include "Transaction.h"
#include "ThreadPool.h"
#include "State.h"
#include <vector>
#include <string>
using namespace std;

class Executor {
public:
    void executeSequential(const DAG &dag);
    void executeParallelBatches(const DAG &dag);
    void executeParallelBatchesWithThreads(const DAG &dag);
    void executePriorityScheduledBatches(const DAG &dag, const vector<Transaction> &txs);
    void executeWithThreadPool(const DAG &dag, const vector<Transaction> &txs);

    // Commit 7: State-aware execution
    // For each batch: evaluate transactions in parallel to compute local deltas,
    // then merge all deltas into the global state.
    void executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize = 4);
};

#endif // EXECUTOR_H