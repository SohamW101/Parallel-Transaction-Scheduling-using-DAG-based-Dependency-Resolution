#ifndef STATE_H
#define STATE_H

#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;

class State {
private:
    unordered_map<string, long long> balances;

public:
    State() = default;
    State(const unordered_map<string, long long> &init);

    long long getBalance(const string &key) const;
    void applyDelta(const unordered_map<string, long long> &delta);
    void display() const;

    // For convenience in Utils
    void setBalance(const string &key, long long value);
};

#endif // STATE_H