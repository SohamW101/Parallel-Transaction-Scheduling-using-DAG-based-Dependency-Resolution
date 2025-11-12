#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "Executor.h"
#include "Utils.h"

int main() {
    std::cout << "=== Parallel Blockchain Verification System ===\n";

    // Create sample transactions
    auto transactions = createSampleTransactions();

    // Display transactions
    for (auto &tx : transactions) {
        tx.display();
    }

    // Build simple DAG (for now manually)
    DAG dag;
    for (auto &tx : transactions) dag.addTransaction(tx);

    dag.addEdge("Tx1", "Tx3"); // Tx3 depends on Tx1
    // Tx2 independent

    dag.displayGraph();

    // Execute sequentially (parallel logic will be added later)
    Executor executor;
    executor.executeSequential(dag);

    std::cout << "\nSetup successful. DAG structure ready for dependency logic.\n";
    return 0;
}