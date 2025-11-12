#include "Executor.h"
#include <iostream>
#include <queue>

// For first commit: simple sequential execution placeholder
void Executor::executeSequential(const DAG &dag) {
    std::cout << "\nExecuting transactions sequentially (placeholder)...\n";

    auto adj = dag.getAdjList();
    for (const auto &pair : adj) {
        std::cout << "  Executing " << pair.first << "\n";
    }
}