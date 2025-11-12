#include "DAG.h"
#include <iostream>

void DAG::addTransaction(const Transaction &tx) {
    if (adjList.find(tx.getId()) == adjList.end()) {
        adjList[tx.getId()] = {};
        inDegree[tx.getId()] = 0;
    }
}

void DAG::addEdge(const std::string &from, const std::string &to) {
    // Prevent duplicate edges
    for (auto &x : adjList[from])
        if (x == to)
            return;

    adjList[from].push_back(to);
    inDegree[to]++;
}

void DAG::buildFromTransactions(const std::vector<Transaction> &txs) {
    // Step 1: Add all transactions as nodes
    for (auto &tx : txs) addTransaction(tx);

    // Step 2: Detect dependencies and add edges
    for (size_t i = 0; i < txs.size(); ++i) {
        for (size_t j = i + 1; j < txs.size(); ++j) {
            const auto &A = txs[i];
            const auto &B = txs[j];

            bool conflictAtoB = false;
            bool conflictBtoA = false;

            // Rule 1: A.write ∩ B.read → A → B
            for (auto &w : A.getWriteSet()) {
                if (B.getReadSet().count(w)) conflictAtoB = true;
            }

            // Rule 2: A.write ∩ B.write → A → B
            for (auto &w : A.getWriteSet()) {
                if (B.getWriteSet().count(w)) conflictAtoB = true;
            }

            // Rule 3: A.read ∩ B.write → B → A
            for (auto &r : A.getReadSet()) {
                if (B.getWriteSet().count(r)) conflictBtoA = true;
            }

            if (conflictAtoB) addEdge(A.getId(), B.getId());
            if (conflictBtoA) addEdge(B.getId(), A.getId());
        }
    }
}

void DAG::displayGraph() const {
    std::cout << "\n DAG Dependency Graph:\n";
    for (auto &pair : adjList) {
        std::cout << "  " << pair.first << " ->";
        for (auto &v : pair.second) std::cout << v << " ";
        std::cout << "\n";
    }
}

const std::unordered_map<std::string, std::vector<std::string>> &DAG::getAdjList() const {
    return adjList;
}

const std::unordered_map<std::string, int> &DAG::getInDegree() const {
    return inDegree;
}