#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "Executor.h"
#include "Utils.h"
#include "Metrics.h"
using namespace std;

int main() {
    srand(time(nullptr));

    auto txs = createSampleTransactions();
    DAG dag;
    dag.buildFromTransactions(txs);

    State state = createInitialState();
    Executor executor;

    Metrics metrics;
    metrics.startGlobalTimer();

    executor.executeWithState(dag, txs, state, 4, metrics);

    long long totalMs = metrics.getElapsedMs();
    long long totalUs = metrics.getElapsedUs();
    cout << "\nTotal execution time: " << totalMs << " ms (" << totalUs << " us)\n\n";

    state.display();

    return 0;
}
