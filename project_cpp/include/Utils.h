#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "Transaction.h"
#include "State.h"
using namespace std;

// Creates sample transactions including fee and timestamp
vector<Transaction> createSampleTransactions();

// Creates an initial state (balances) for the demo
State createInitialState();

#endif // UTILS_H