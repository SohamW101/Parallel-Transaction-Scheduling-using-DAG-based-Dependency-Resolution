#include "Transaction.h"

Transaction::Transaction(const string &txId,
                         const unordered_set<string> &rSet,
                         const unordered_set<string> &wSet,
                         int f,
                         long long ts)
    : id(txId), readSet(rSet), writeSet(wSet), fee(f), timestamp(ts) {}

const string &Transaction::getId() const { return id; }
const unordered_set<string> &Transaction::getReadSet() const { return readSet; }
const unordered_set<string> &Transaction::getWriteSet() const { return writeSet; }
int Transaction::getFee() const { return fee; }
long long Transaction::getTimestamp() const { return timestamp; }

void Transaction::display() const {
    cout << "Transaction ID: " << id << "\n";
    cout << "  Fee: " << fee << "\n";
    cout << "  Timestamp: " << timestamp << "\n";
    cout << "  Read Set: ";
    for (auto &r : readSet) cout << r << " ";
    cout << "\n  Write Set: ";
    for (auto &w : writeSet) cout << w << " ";
    cout << "\n";
}