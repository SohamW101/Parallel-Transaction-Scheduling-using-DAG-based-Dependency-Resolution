// ExecutionObserver.h
#pragma once
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

using TxDelta = std::unordered_map<std::string, long long>;

struct ExecutionObserver {
    // batchId, list of tx ids in batch
    std::function<void(int, const std::vector<std::string>&)> onBatchStart;
    // batchId, groupId, list of tx ids in group
    std::function<void(int, int, const std::vector<std::string>&)> onGroupStart;
    // txId, threadId (as string), delta map
    std::function<void(const std::string&, const std::string&, const TxDelta&)> onTxEvaluated;
    // batchId, groupId, merged delta
    std::function<void(int, int, const TxDelta&)> onGroupMerged;
    // execution finished
    std::function<void()> onExecutionEnd;
};
