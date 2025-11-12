#include "Transaction.h"

Transaction::Transaction(const std::string &txId,
                         const std::unordered_set<std::string> &rSet,
                         const std::unordered_set<std::string> &wSet)
    : id(txId), readSet(rSet), writeSet(wSet) {}

const std::string &Transaction::getId() const { return id; }

const std::unordered_set<std::string> &Transaction::getReadSet() const {
    return readSet;
}

const std::unordered_set<std::string> &Transaction::getWriteSet() const {
    return writeSet;
}

void Transaction::display() const {
    std::cout << "Transaction ID: " << id << "\n";
    std::cout << "  Read Set: ";
    for (auto &r : readSet) std::cout << r << " ";
    std::cout << "\n  Write Set: ";
    for (auto &w : writeSet) std::cout << w << " ";
    std::cout << "\n";
}