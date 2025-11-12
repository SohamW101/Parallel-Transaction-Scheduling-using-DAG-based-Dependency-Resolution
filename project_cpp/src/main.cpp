#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "Executor.h"
#include "Utils.h"

int main() {
    std::cout << "=== Parallel Blockchain Verification System ===\n";

    auto transactions = createSampleTransactions();

    std::cout << "\n Transactions in Block:\n";
    for (auto &tx : transactions) tx.display();

    DAG dag;
    dag.buildFromTransactions(transactions);

    dag.displayGraph();

    Executor executor;
    executor.executeSequential(dag);

    std::cout << "\n DAG construction and sequential topological execution complete.\n";
    return 0;
}