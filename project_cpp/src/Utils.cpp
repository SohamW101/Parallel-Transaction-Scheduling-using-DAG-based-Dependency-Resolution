#include "Utils.h"
using namespace std;

// Dummy sample transactions with fees and timestamps
vector<Transaction> createSampleTransactions() {
    vector<Transaction> txs;

    // Tx1: Writes to B → Tx3 depends on it
    txs.emplace_back(
        "Tx1",
        unordered_set<string>{"A"},          // read set
        unordered_set<string>{"B"},          // write set
        15,                                   // fee
        1001                                  // timestamp
    );

    // Tx2: Independent
    txs.emplace_back(
        "Tx2",
        unordered_set<string>{"C"},
        unordered_set<string>{"D"},
        25,                                   // higher fee than Tx1
        1000                                  // earlier timestamp
    );

    // Tx3: Depends on Tx1 (reads B)
    txs.emplace_back(
        "Tx3",
        unordered_set<string>{"B"},
        unordered_set<string>{"E"},
        10,                                   // lowest fee
        1002                                  // latest timestamp
    );

    return txs;
}