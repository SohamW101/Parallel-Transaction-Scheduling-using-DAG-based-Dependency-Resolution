// Executor.cpp
#include "Executor.h"
#include "Metrics.h"
#include "ExecutionObserver.h"
#include "TraceWriter.h"

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>

using namespace std;

static mutex coutMutex;

static bool transactionsConflict(const Transaction &A, const Transaction &B) {
    for (const auto &w : A.getWriteSet()) {
        if (B.getReadSet().count(w)) return true;
    }
    for (const auto &w : A.getWriteSet()) {
        if (B.getWriteSet().count(w)) return true;
    }
    for (const auto &r : A.getReadSet()) {
        if (B.getWriteSet().count(r)) return true;
    }
    return false;
}

static vector<vector<string>> partitionIntoConflictFreeGroups(
    const vector<string> &batch,
    const unordered_map<string, Transaction> &lookup
) {
    vector<vector<string>> groups;
    for (const auto &txid : batch) {
        const Transaction &tx = lookup.at(txid);
        bool placed = false;
        for (auto &group : groups) {
            bool conflict_with_group = false;
            for (const auto &memberId : group) {
                const Transaction &memberTx = lookup.at(memberId);
                if (transactionsConflict(tx, memberTx) || transactionsConflict(memberTx, tx)) {
                    conflict_with_group = true;
                    break;
                }
            }
            if (!conflict_with_group) {
                group.push_back(txid);
                placed = true;
                break;
            }
        }
        if (!placed) groups.push_back(vector<string>{txid});
    }
    return groups;
}

// ---- JSON helpers for trace events ----
static std::string escapeJsonString(const std::string &s) {
    std::ostringstream o;
    for (auto c : s) {
        switch (c) {
            case '\"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) <= 0x1f) {
                    o << "\\u"
                      << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    o << c;
                }
        }
    }
    return o.str();
}

static std::string deltaToJson(const unordered_map<string, long long> &d) {
    std::ostringstream o;
    o << "{";
    bool first = true;
    for (const auto &p : d) {
        if (!first) o << ",";
        first = false;
        o << "\"" << escapeJsonString(p.first) << "\":" << p.second;
    }
    o << "}";
    return o.str();
}

void Executor::executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize, Metrics &metrics) {
    cout << "\nState-aware execution with conflict detection + PERFORMANCE METRICS\n";

    metrics.log("=== Execution Start ===");

    // adjacency
    auto adj = dag.getAdjList();

    // build lookup
    unordered_map<string, Transaction> lookup;
    for (auto &tx : txs) lookup[tx.getId()] = tx;

    // IMPORTANT: initialize indegree for ALL transactions (not only keys found in adj)
    unordered_map<string, int> indeg;
    for (const auto &t : txs) {
        indeg[t.getId()] = 0;
    }
    // now increment indegree from adjacency
    for (const auto &p : adj) {
        for (const auto &to : p.second) {
            // ensure both endpoints exist in indeg map
            if (!indeg.count(p.first)) indeg[p.first] = 0;
            if (!indeg.count(to)) indeg[to] = 0;
            indeg[to] += 1;
        }
    }

    ThreadPool pool(threadPoolSize);

    // initial zero-indegree batch: collect all txids with indegree == 0
    vector<string> batch;
    for (auto &p : indeg) {
        if (p.second == 0) batch.push_back(p.first);
    }

    int batchNum = 1;

    while (!batch.empty()) {

        { lock_guard<mutex> lock(coutMutex);
          cout << "\nBatch " << batchNum << " size = " << batch.size() << "\n"; }
        metrics.log("Batch " + to_string(batchNum) + " size=" + to_string(batch.size()));

        // Notify observer & trace for batch start
        if (observer.onBatchStart) observer.onBatchStart(batchNum, batch);
        {
            std::ostringstream e;
            e << "{\"type\":\"batch_start\",\"batchId\":" << batchNum << ",\"batch\":[";
            for (size_t i = 0; i < batch.size(); ++i) {
                if (i) e << ",";
                e << "\"" << escapeJsonString(batch[i]) << "\"";
            }
            e << "]}";
            TraceWriter::get().pushEvent(e.str());
        }

        long long batchTime = metrics.measureDuration([&]() {

            auto groups = partitionIntoConflictFreeGroups(batch, lookup);
            metrics.log("    Group count=" + to_string(groups.size()));

            int groupNum = 1;
            for (auto &group : groups) {
                { lock_guard<mutex> lock(coutMutex);
                  cout << "  Group " << groupNum << " (parallel size = " << group.size() << ")\n"; }

                // Notify observer & trace for group start
                if (observer.onGroupStart) observer.onGroupStart(batchNum, groupNum, group);
                {
                    std::ostringstream e;
                    e << "{\"type\":\"group_start\",\"batchId\":"<<batchNum<<",\"groupId\":"<<groupNum<<",\"group\":[";
                    for (size_t i = 0; i < group.size(); ++i) {
                        if (i) e << ",";
                        e << "\"" << escapeJsonString(group[i]) << "\"";
                    }
                    e << "]}";
                    TraceWriter::get().pushEvent(e.str());
                }

                long long groupTime = metrics.measureDuration([&]() {

                    vector<unordered_map<string, long long>> localDeltas;
                    localDeltas.reserve(group.size());
                    mutex deltasMutex;

                    for (auto &txID : group) {
                        pool.enqueue([&, txID]() {
                            unordered_map<string, long long> delta;
                            const Transaction &t = lookup.at(txID);
                            string from = ""; string to = "";
                            if (!t.getReadSet().empty()) from = *t.getReadSet().begin();
                            if (!t.getWriteSet().empty()) to = *t.getWriteSet().begin();
                            if (!from.empty() && !to.empty()) {
                                delta[from]--;
                                delta[to]++;
                            }

                            { lock_guard<mutex> lock(deltasMutex);
                              localDeltas.push_back(delta); }

                            { lock_guard<mutex> lock(coutMutex);
                              cout << "    Evaluated " << txID << " on thread " << this_thread::get_id() << "\n"; }

                            // observer + trace for tx evaluated
                            try {
                                size_t thh = std::hash<std::thread::id>{}(std::this_thread::get_id());
                                std::string threadIdStr = std::to_string(thh);
                                if (observer.onTxEvaluated) {
                                    unordered_map<string,long long> copyDelta = delta;
                                    observer.onTxEvaluated(txID, threadIdStr, copyDelta);
                                }

                                std::ostringstream e;
                                e << "{\"type\":\"tx_eval\",\"txId\":\"" << escapeJsonString(txID) << "\",";
                                e << "\"threadId\":\"" << threadIdStr << "\",";
                                e << "\"delta\":" << deltaToJson(delta) << "}";
                                TraceWriter::get().pushEvent(e.str());
                            } catch (...) {
                                // swallow
                            }
                        });
                    }

                    pool.waitAll();

                    unordered_map<string, long long> merged;
                    for (auto &d : localDeltas)
                        for (auto &p : d) merged[p.first] += p.second;

                    { lock_guard<mutex> lock(coutMutex);
                      cout << "    Merged group delta into global state\n"; }

                    if (observer.onGroupMerged) observer.onGroupMerged(batchNum, groupNum, merged);
                    {
                        std::ostringstream e;
                        e << "{\"type\":\"group_merged\",\"batchId\":"<<batchNum<<",\"groupId\":"<<groupNum<<",\"merged\":"<<deltaToJson(merged)<<"}";
                        TraceWriter::get().pushEvent(e.str());
                    }

                    // apply merged delta
                    state.applyDelta(merged);

                });

                metrics.log("        Group " + to_string(groupNum) + " time=" + to_string(groupTime) + "ms");
                groupNum++;
            }

        });

        metrics.log("Batch " + to_string(batchNum) + " duration=" + to_string(batchTime) + "ms");

        // compute next batch: walk edges from nodes in current batch and decrement indeg
        vector<string> nextBatch;
        for (auto &tx : batch) {
            if (adj.count(tx)) {
                for (auto &nbr : adj.at(tx)) {
                    // ensure indeg entry exists (defensive)
                    if (!indeg.count(nbr)) indeg[nbr] = 0;
                    indeg[nbr] -= 1;
                    if (indeg[nbr] == 0) nextBatch.push_back(nbr);
                }
            }
        }

        batch = move(nextBatch);
        batchNum++;
    }

    metrics.log("=== Execution End ===");
    if (observer.onExecutionEnd) observer.onExecutionEnd();
    TraceWriter::get().pushEvent("{\"type\":\"execution_end\"}");

    cout << "\nExecution with metrics complete.\n";
}
