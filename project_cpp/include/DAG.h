#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>
#include <string>
#include "Transaction.h"

class DAG {
private:
    std::unordered_map<std::string, std::vector<std::string>> adjList;
    std::unordered_map<std::string, int> inDegree;

public:
    void addTransaction(const Transaction &tx);
    void addEdge(const std::string &from, const std::string &to);
    void displayGraph() const;

    const std::unordered_map<std::string, std::vector<std::string>> &getAdjList() const;
    const std::unordered_map<std::string, int> &getInDegree() const;
};

#endif // DAG_H