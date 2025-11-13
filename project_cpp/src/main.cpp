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
    for (auto &tx : transactions) tx.display();

    DAG dag;
    dag.buildFromTransactions(transactions);
    dag.displayGraph();

    Executor executor;

    cout << "\n===================================";
    cout << "\n Mode 6: Thread Pool Execution";
    cout << "\n===================================";
    executor.executeWithThreadPool(dag, transactions);

    return 0;
}