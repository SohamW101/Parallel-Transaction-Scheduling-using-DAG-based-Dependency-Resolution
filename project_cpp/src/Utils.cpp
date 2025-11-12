#include "Utils.h"

// Simple dummy data for testing setup
std::vector<Transaction> createSampleTransactions() {
    std::vector<Transaction> txs;

    txs.emplace_back("Tx1", std::unordered_set<std::string>{"A"}, std::unordered_set<std::string>{"B"});
    txs.emplace_back("Tx2", std::unordered_set<std::string>{"C"}, std::unordered_set<std::string>{"D"});
    txs.emplace_back("Tx3", std::unordered_set<std::string>{"B"}, std::unordered_set<std::string>{"E"});

    return txs;
}