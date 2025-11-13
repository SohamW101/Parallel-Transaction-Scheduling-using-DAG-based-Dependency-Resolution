#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <unordered_set>
#include <iostream>
using namespace std;

class Transaction {
private:
    string id;
    unordered_set<string> readSet;
    unordered_set<string> writeSet;
    int fee;
    long long timestamp;

public:
    Transaction() = default;

    Transaction(const string &txId,
                const unordered_set<string> &readSet,
                const unordered_set<string> &writeSet,
                int fee,
                long long timestamp);

    const string &getId() const;
    const unordered_set<string> &getReadSet() const;
    const unordered_set<string> &getWriteSet() const;
    int getFee() const;
    long long getTimestamp() const;

    void display() const;
};

#endif // TRANSACTION_H