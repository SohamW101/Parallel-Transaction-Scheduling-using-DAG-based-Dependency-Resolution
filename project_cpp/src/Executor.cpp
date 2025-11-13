#include "Executor.h"
#include "Metrics.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <functional>
using namespace std;

// single mutex definition for console printing in this translation unit
static mutex coutMutex;

// helper: check conflict between two transactions using read/write sets
static bool transactionsConflict(const Transaction &A, const Transaction &B) {
    // A.write ∩ B.read
    for (const auto &w : A.getWriteSet()) {
        if (B.getReadSet().count(w)) return true;
    }
    // A.write ∩ B.write
    for (const auto &w : A.getWriteSet()) {
        if (B.getWriteSet().count(w)) return true;
    }
    // A.read ∩ B.write
    for (const auto &r : A.getReadSet()) {
        if (B.getWriteSet().count(r)) return true;
    }
    return false;
}

// Partition a batch of txIDs into conflict-free groups (greedy)
static vector<vector<string>> partitionIntoConflictFreeGroups(
    const vector<string> &batch,
    const unordered_map<string, Transaction> &lookup
) {
    vector<vector<string>> groups;

    for (const auto &txid : batch) {
        const Transaction &tx = lookup.at(txid);
        bool placed = false;
        // try to place into an existing group
        for (auto &group : groups) {
            bool conflict_with_group = false;
            for (const auto &memberId : group) {
                const Transaction &memberTx = lookup.at(memberId);
                // check both directions (conservative)
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
        if (!placed) {
            groups.push_back(vector<string>{txid});
        }
    }
    return groups;
}

// === state-aware execution with conflict detection and metrics ===
void Executor::executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize, Metrics &metrics) {
    cout << "\nState-aware execution with conflict detection + PERFORMANCE METRICS\n";

    metrics.log("=== Execution Start ===");

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    unordered_map<string, Transaction> lookup;
    for (auto &tx : txs)
        lookup[tx.getId()] = tx;

    ThreadPool pool(threadPoolSize);

    vector<string> batch;
    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    int batchNum = 1;

    while (!batch.empty()) {

        { // print batch header to console and log
            lock_guard<mutex> lock(coutMutex);
            cout << "\nBatch " << batchNum << " size = " << batch.size() << "\n";
        }
        metrics.log("Batch " + to_string(batchNum) + " size=" + to_string(batch.size()));

        long long batchTime = metrics.measureDuration([&]() {

            auto groups = partitionIntoConflictFreeGroups(batch, lookup);
            metrics.log("    Group count=" + to_string(groups.size()));

            int groupNum = 1;

            for (auto &group : groups) {
                { // print group header
                    lock_guard<mutex> lock(coutMutex);
                    cout << "  Group " << groupNum << " (parallel size = " << group.size() << ")\n";
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

                            {
                                lock_guard<mutex> lock(deltasMutex);
                                localDeltas.push_back(move(delta));
                            }

                            {
                                lock_guard<mutex> lock(coutMutex);
                                cout << "    Evaluated " << txID << " on thread " << this_thread::get_id() << "\n";
                            }
                        });
                    }

                    pool.waitAll();

                    unordered_map<string, long long> merged;
                    for (auto &d : localDeltas)
                        for (auto &p : d)
                            merged[p.first] += p.second;

                    // Print merged message before applying to state for clarity
                    {
                        lock_guard<mutex> lock(coutMutex);
                        cout << "    Merged group delta into global state\n";
                    }

                    state.applyDelta(merged);

                }); // end measureDuration for group

                metrics.log("        Group " + to_string(groupNum) + " time=" + to_string(groupTime) + "ms");
                groupNum++;
            }

        }); // end measureDuration for batch

        metrics.log("Batch " + to_string(batchNum) + " duration=" + to_string(batchTime) + "ms");

        // Next batch calculation
        vector<string> nextBatch;
        for (auto &tx : batch) {
            for (auto &nbr : adj.at(tx)) {
                if (--indeg[nbr] == 0) nextBatch.push_back(nbr);
            }
        }

        batch = move(nextBatch);
        batchNum++;
    }

    metrics.log("=== Execution End ===");
    cout << "\nExecution with metrics complete.\n";
}
