#ifndef DAG_H
#define DAG_H

#include <unordered_map>
#include <vector>
#include <string>
#include <unordered_set>
#include "Transaction.h"
using namespace std;

class DAG {
private:
    unordered_map<string, vector<string>> adj;
    unordered_map<string, int> indegree;

public:
    DAG() = default;

    void addNode(const string &id);

    // addEdge now prevents duplicates
    void addEdge(const string &from, const string &to);

    void buildFromTransactions(const vector<Transaction> &txs);

    const unordered_map<string, vector<string>>& getAdjList() const { return adj; }
    const unordered_map<string, int>& getInDegree() const { return indegree; }

    void displayGraph() const;

    vector<string> getAllNodes() const;
};

#endif // DAG_H