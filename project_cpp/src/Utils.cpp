#include "Utils.h"
using namespace std;

// Dummy sample transactions with fees and timestamps
vector<Transaction> createSampleTransactions() {
    vector<Transaction> txs;

    // Tx1: transfer A -> B
    txs.emplace_back(
        "Tx1",
        unordered_set<string>{"A"},          // read set (sender)
        unordered_set<string>{"B"},          // write set (receiver)
        15,                                   // fee
        1001                                  // timestamp
    );

    // Tx2: transfer C -> D (higher fee)
    txs.emplace_back(
        "Tx2",
        unordered_set<string>{"C"},
        unordered_set<string>{"D"},
        25,
        1000
    );

    // Tx3: transfer B -> E (depends on Tx1)
    txs.emplace_back(
        "Tx3",
        unordered_set<string>{"B"},
        unordered_set<string>{"E"},
        10,
        1002
    );

    return txs;
}

State createInitialState() {
    unordered_map<string, long long> init;
    init["A"] = 10;
    init["B"] = 0;
    init["C"] = 5;
    init["D"] = 0;
    init["E"] = 0;
    return State(init);
}