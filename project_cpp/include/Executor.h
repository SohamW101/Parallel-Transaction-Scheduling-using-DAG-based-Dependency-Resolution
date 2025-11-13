#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "DAG.h"
#include "Transaction.h"
#include "ThreadPool.h"
#include "State.h"
#include "Metrics.h"
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

    // Commit 7/8 version
    void executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize, Metrics &metrics);
};

#endif // EXECUTOR_H