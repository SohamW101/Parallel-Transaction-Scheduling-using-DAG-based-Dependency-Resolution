#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "Executor.h"
#include "Utils.h"
using namespace std;

int main() {
    srand(time(nullptr));
    cout << "=== Parallel Blockchain Verification System ===\n";

    auto transactions = createSampleTransactions();

    cout << "\nTransactions in Block:\n";
    for (auto &tx : transactions) tx.display();

    DAG dag;
    dag.buildFromTransactions(transactions);
    dag.displayGraph();

    Executor executor;

    cout << "\n==========================";
    cout << "\n Mode 1: Sequential Execution";
    cout << "\n==========================";
    executor.executeSequential(dag);

    cout << "\n==========================";
    cout << "\n Mode 2: Parallel Batch Simulation";
    cout << "\n==========================";
    executor.executeParallelBatches(dag);

    cout << "\n==========================";
    cout << "\n Mode 3: Real Multithreaded Execution";
    cout << "\n==========================";
    executor.executeParallelBatchesWithThreads(dag);

    cout << "\nParallel multithreaded execution demo complete.\n";
    return 0;
}