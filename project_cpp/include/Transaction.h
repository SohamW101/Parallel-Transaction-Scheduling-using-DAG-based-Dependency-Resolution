#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <unordered_set>
#include <iostream>

class Transaction {
private:
    std::string id;
    std::unordered_set<std::string> readSet;
    std::unordered_set<std::string> writeSet;

public:
    Transaction() = default;
    Transaction(const std::string &txId,
                const std::unordered_set<std::string> &readSet,
                const std::unordered_set<std::string> &writeSet);

    const std::string &getId() const;
    const std::unordered_set<std::string> &getReadSet() const;
    const std::unordered_set<std::string> &getWriteSet() const;

    void display() const;
};

#endif // TRANSACTION_H