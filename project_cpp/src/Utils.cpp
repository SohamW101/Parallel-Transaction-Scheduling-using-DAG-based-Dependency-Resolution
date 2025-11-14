// Utils.cpp
#include "Utils.h"
using namespace std;

vector<Transaction> createSampleTransactions() {
    vector<Transaction> txs;

    txs.emplace_back("Tx1",
        unordered_set<string>{"A"},
        unordered_set<string>{"B"},
        10, 1001);

    txs.emplace_back("Tx2",
        unordered_set<string>{"C"},
        unordered_set<string>{"D"},
        12, 1002);

    txs.emplace_back("Tx3",
        unordered_set<string>{"B"},
        unordered_set<string>{"E"},
        9, 1003);

    txs.emplace_back("Tx4",
        unordered_set<string>{"F"},
        unordered_set<string>{"G"},
        7, 1004);

    txs.emplace_back("Tx5",
        unordered_set<string>{"H"},
        unordered_set<string>{"I"},
        6, 1005);

    txs.emplace_back("Tx6",
        unordered_set<string>{"E"},
        unordered_set<string>{"J"},
        8, 1006);

    txs.emplace_back("Tx7",
        unordered_set<string>{"D"},
        unordered_set<string>{"H"},
        11, 1007);

    txs.emplace_back("Tx8",
        unordered_set<string>{"J"},
        unordered_set<string>{"Z"},   // FIXED: was writing back to A
        5, 1008);

    txs.emplace_back("Tx9",
        unordered_set<string>{"K"},
        unordered_set<string>{"L"},
        4, 1009);

    txs.emplace_back("Tx10",
        unordered_set<string>{"L"},
        unordered_set<string>{"C"},
        3, 1010);

    return txs;
}

State createInitialState() {
    unordered_map<string, long long> init;
    init["A"] = 10;
    init["B"] = 0;
    init["C"] = 5;
    init["D"] = 0;
    init["E"] = 0;
    init["F"] = 7;
    init["G"] = 0;
    init["H"] = 0;
    init["I"] = 0;
    init["J"] = 0;
    init["K"] = 6;
    init["L"] = 0;
    init["Z"] = 0;   // new account for Tx8
    return State(init);
}
