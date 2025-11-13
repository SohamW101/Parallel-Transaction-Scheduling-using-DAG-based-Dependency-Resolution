#include "DAG.h"
#include <iostream>
#include <unordered_set>
using namespace std;

void DAG::addNode(const string &id) {
    if (!adj.count(id)) {
        adj[id] = {};
        indegree[id] = 0;
    }
}

// ⛔ FIX: Prevent duplicate edges
void DAG::addEdge(const string &from, const string &to) {
    auto &vec = adj[from];
    for (const string &existing : vec) {
        if (existing == to) return;  // already exists → do not add again
    }
    vec.push_back(to);
    indegree[to]++;
}

void DAG::buildFromTransactions(const vector<Transaction> &txs) {
    // Add all nodes
    for (const auto &tx : txs) addNode(tx.getId());

    // Build dependencies safely
    for (size_t i = 0; i < txs.size(); i++) {
        for (size_t j = 0; j < txs.size(); j++) {
            if (i == j) continue;

            const Transaction &A = txs[i];
            const Transaction &B = txs[j];

            bool A_to_B = false;
            bool B_to_A = false;

            // Rule 1: A.write ∩ B.read → A→B
            for (auto &w : A.getWriteSet())
                if (B.getReadSet().count(w)) A_to_B = true;

            // Rule 2: A.write ∩ B.write → A→B
            for (auto &w : A.getWriteSet())
                if (B.getWriteSet().count(w)) A_to_B = true;

            // Rule 3: A.read ∩ B.write → B→A
            for (auto &r : A.getReadSet())
                if (B.getWriteSet().count(r)) B_to_A = true;

            // Apply edges
            if (A_to_B && !B_to_A) addEdge(A.getId(), B.getId());
            if (B_to_A && !A_to_B) addEdge(B.getId(), A.getId());

            // Very rare case: both true → prefer write-order
            if (A_to_B && B_to_A)
                addEdge(A.getId(), B.getId());
        }
    }
}

void DAG::displayGraph() const {
    cout << "\nDAG Dependency Graph:\n";
    for (auto &p : adj) {
        cout << "  " << p.first << " → ";
        for (auto &t : p.second) cout << t << " ";
        cout << "\n";
    }
}

vector<string> DAG::getAllNodes() const {
    vector<string> nodes;
    nodes.reserve(adj.size());
    for (auto &p : adj) nodes.push_back(p.first);
    return nodes;
}