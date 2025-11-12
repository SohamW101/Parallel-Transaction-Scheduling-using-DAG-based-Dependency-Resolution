#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "DAG.h"
#include <vector>
#include <string>

class Executor {
public:
    void executeSequential(const DAG &dag);
};

#endif // EXECUTOR_H