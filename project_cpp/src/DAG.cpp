#include "DAG.h"
#include <iostream>

void DAG::addTransaction(const Transaction &tx) {
    if (adjList.find(tx.getId()) == adjList.end()) {
        adjList[tx.getId()] = {};
        inDegree[tx.getId()] = 0;
    }
}

void DAG::addEdge(const std::string &from, const std::string &to) {
    adjList[from].push_back(to);
    inDegree[to]++;
}

void DAG::displayGraph() const {
    std::cout << "\nCurrent DAG Structure:\n";
    for (auto &pair : adjList) {
        std::cout << "  " << pair.first << " -> ";
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