#include "Executor.h"
#include <iostream>
#include <queue>
#include <unordered_map>

// Simple placeholder for now: executes transactions in topological order if possible
void Executor::executeSequential(const DAG &dag) {
    std::cout << "\n⚙️ Executing transactions in topological order (Commit #2)...\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    // Simple topological traversal (Kahn's Algorithm - sequential)
    std::queue<std::string> q;

    for (auto &p : indeg) {
        if (p.second == 0) q.push(p.first);
    }

    int step = 1;
    while (!q.empty()) {
        std::string tx = q.front(); q.pop();
        std::cout << "  Step " << step++ << ": Executing " << tx << "\n";

        for (auto &nbr : adj.at(tx)) {
            indeg[nbr]--;
            if (indeg[nbr] == 0) q.push(nbr);
        }
    }

    std::cout << "✅ Execution complete.\n";
}