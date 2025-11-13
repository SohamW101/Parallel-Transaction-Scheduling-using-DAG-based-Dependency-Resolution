#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "Executor.h"
#include "Utils.h"
using namespace std;

int main() {
    srand(time(nullptr));
    cout << "=== Parallel Blockchain Verification System (Commit 7) ===\n";

    auto transactions = createSampleTransactions();

    cout << "\nTransactions in Block:\n";
    for (auto &tx : transactions) tx.display();

    DAG dag;
    dag.buildFromTransactions(transactions);
    dag.displayGraph();

    State state = createInitialState();
    cout << "\nInitial ";
    state.display();

    Executor executor;

    cout << "\n===================================";
    cout << "\n Mode 7: State-aware Parallel Execution (batch evaluation + merge)";
    cout << "\n===================================\n";
    executor.executeWithState(dag, transactions, state, 4);

    cout << "\nFinal ";
    state.display();

    return 0;
}