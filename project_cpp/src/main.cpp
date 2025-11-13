#include <iostream>
#include "Transaction.h"
#include "DAG.h"
#include "DAGExporter.h"
#include "Executor.h"
#include "State.h"
#include "Metrics.h"
#include "Utils.h"
using namespace std;

int main() {
    cout << "=== Parallel Blockchain System — (DAG Export) ===\n";

    auto txs = createSampleTransactions();
    DAG dag;
    dag.buildFromTransactions(txs);
    dag.displayGraph();

    // Export DAG
    DAGExporter::exportToDOT(dag, "dag_output.dot");
    DAGExporter::exportToJSON(dag, "dag_output.json");

    State state = createInitialState();
    Executor executor;
    Metrics metrics;
    metrics.startGlobalTimer();

    executor.executeWithState(dag, txs, state, 4, metrics);

    cout << "\nTotal execution time: "
         << metrics.getElapsedMs() << " ms ("
         << metrics.getElapsedUs() << " us)\n";

    state.display();

    return 0;
}