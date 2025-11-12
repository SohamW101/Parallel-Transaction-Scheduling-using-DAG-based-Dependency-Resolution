#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "DAG.h"
#include <vector>
#include <string>
using namespace std;

class Executor {
public:
    void executeSequential(const DAG &dag);
    void executeParallelBatches(const DAG &dag);
};

#endif // EXECUTOR_H