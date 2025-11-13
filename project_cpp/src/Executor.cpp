#include "Executor.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

mutex coutMutex;

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

// Updated state-aware execution that uses conflict-free grouping inside batches
void Executor::executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize) {
    cout << "\nState-aware execution with conflict detection and safe parallel updates\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    unordered_map<string, Transaction> lookup;
    for (const auto &tx : txs)
        lookup[tx.getId()] = tx;

    // initial batch
    vector<string> batch;
    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    ThreadPool pool(threadPoolSize);
    int batchNum = 1;

    while (!batch.empty()) {
        cout << "\nBatch " << batchNum++ << " : size = " << batch.size() << "\n";

        // Partition batch into conflict-free groups
        auto groups = partitionIntoConflictFreeGroups(batch, lookup);
        int groupNum = 1;
        for (auto &group : groups) {
            cout << "  Group " << groupNum++ << " (parallel size = " << group.size() << ")\n";

            // Evaluate transactions in group in parallel, collect local deltas
            vector<unordered_map<string, long long>> localDeltas;
            localDeltas.reserve(group.size());
            mutex deltasMutex;

            for (auto &txID : group) {
                pool.enqueue([&, txID]() {
                    unordered_map<string, long long> delta;
                    const Transaction &t = lookup.at(txID);

                    // For demo: single-unit transfer from first read -> first write
                    string from = "";
                    string to = "";
                    if (!t.getReadSet().empty()) from = *(t.getReadSet().begin());
                    if (!t.getWriteSet().empty()) to = *(t.getWriteSet().begin());

                    if (!from.empty() && !to.empty()) {
                        delta[from] -= 1;
                        delta[to] += 1;
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

            // Wait for group tasks to finish
            pool.waitAll();

            // Merge group deltas into batchDelta (sequential merge for atomicity)
            unordered_map<string, long long> groupDelta;
            for (auto &d : localDeltas) {
                for (auto &p : d) groupDelta[p.first] += p.second;
            }

            // Apply group's delta to global state
            state.applyDelta(groupDelta);
            {
                lock_guard<mutex> lock(coutMutex);
                cout << "    Merged group delta into global state\n";
            }
        } // end groups

        // After all groups in the batch processed and merged, compute next batch
        vector<string> nextBatch;
        for (auto &txID : batch) {
            for (auto &nbr : adj.at(txID)) {
                indeg[nbr]--;
                if (indeg[nbr] == 0) nextBatch.push_back(nbr);
            }
        }
        batch = nextBatch;
    } // end while

    cout << "\nConflict-aware state execution finished.\n";
}