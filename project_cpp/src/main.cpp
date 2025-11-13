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
    cout << "\n Mode 4: Priority-Based Scheduling";
    cout << "\n==========================";
    executor.executePriorityScheduledBatches(dag, transactions);

    return 0;
}