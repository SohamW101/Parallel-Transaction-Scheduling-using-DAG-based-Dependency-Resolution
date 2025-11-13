#include "State.h"
using namespace std;

State::State(const unordered_map<string, long long> &init) : balances(init) {}

long long State::getBalance(const string &key) const {
    auto it = balances.find(key);
    if (it == balances.end()) return 0;
    return it->second;
}

void State::applyDelta(const unordered_map<string, long long> &delta) {
    for (auto &p : delta) {
        balances[p.first] += p.second;
    }
}

void State::display() const {
    cout << "\nGlobal State (balances):\n";
    for (auto &p : balances) {
        cout << "  " << p.first << " : " << p.second << "\n";
    }
}

void State::setBalance(const string &key, long long value) {
    balances[key] = value;
}