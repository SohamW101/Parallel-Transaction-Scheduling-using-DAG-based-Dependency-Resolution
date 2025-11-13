#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "DAG.h"
#include "Transaction.h"
#include <vector>
#include <string>
using namespace std;

class Executor {
public:
    void executeSequential(const DAG &dag);
    void executeParallelBatches(const DAG &dag);
    void executeParallelBatchesWithThreads(const DAG &dag);
    void executePriorityScheduledBatches(const DAG &dag, const vector<Transaction> &txs);
};

#endif // EXECUTOR_H