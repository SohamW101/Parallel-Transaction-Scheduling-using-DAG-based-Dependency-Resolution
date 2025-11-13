#include "Executor.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

mutex coutMutex;

// (previous functions: executeSequential, executeParallelBatches, etc.)
// For brevity assume earlier functions remain; we focus on the new executeWithState
// (but keep them in the real file — here we include only executeWithState for clarity)

void Executor::executeWithState(DAG &dag, vector<Transaction> &txs, State &state, size_t threadPoolSize) {
    cout << "\nState-aware execution: parallel evaluation + batch merge\n";

    auto adj = dag.getAdjList();
    auto indeg = dag.getInDegree();

    unordered_map<string, Transaction> lookup;
    for (const auto &tx : txs)
        lookup[tx.getId()] = tx;

    // Find initial ready batch
    vector<string> batch;
    for (auto &p : indeg)
        if (p.second == 0)
            batch.push_back(p.first);

    ThreadPool pool(threadPoolSize);
    int batchNum = 1;

    while (!batch.empty()) {
        cout << "\nBatch " << batchNum++ << " : evaluating " << batch.size() << " tx(s) in parallel\n";

        // Each tx will produce a local delta map<string, long long>
        // We collect them in a thread-safe vector
        vector<unordered_map<string, long long>> localDeltas;
        localDeltas.reserve(batch.size());
        mutex deltasMutex;

        for (auto &txID : batch) {
            pool.enqueue([&, txID]() {
                // Evaluate transaction semantics (demo):
                // interpret transaction as transfer of 1 unit from first read key -> first write key
                unordered_map<string, long long> delta;
                const Transaction &t = lookup[txID];

                // get first read and write keys if they exist
                string from = "";
                string to = "";
                if (!t.getReadSet().empty()) from = *(t.getReadSet().begin());
                if (!t.getWriteSet().empty()) to = *(t.getWriteSet().begin());

                if (!from.empty() && !to.empty()) {
                    // For demo, we don't enforce balance checks here;
                    // we just create the delta: -1 from 'from', +1 to 'to'
                    delta[from] -= 1;
                    delta[to] += 1;
                } else {
                    // no-op transaction; leave delta empty
                }

                {
                    lock_guard<mutex> lock(deltasMutex);
                    localDeltas.push_back(move(delta));
                }

                {
                    lock_guard<mutex> lock(coutMutex);
                    cout << "  Evaluated " << txID << " on thread " << this_thread::get_id() << "\n";
                }
            });
        }

        // Wait until all evaluations finished
        pool.waitAll();

        // Merge deltas into a single batch delta to apply atomically
        unordered_map<string, long long> batchDelta;
        for (auto &d : localDeltas) {
            for (auto &p : d) batchDelta[p.first] += p.second;
        }

        // Apply batchDelta to global state (sequential atomic merge)
        state.applyDelta(batchDelta);
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "  Merged batch delta into global state\n";
        }

        // Compute next batch using adj/indeg
        vector<string> nextBatch;
        for (auto &txID : batch) {
            for (auto &nbr : adj.at(txID)) {
                indeg[nbr]--;
                if (indeg[nbr] == 0)
                    nextBatch.push_back(nbr);
            }
        }
        batch = nextBatch;
    }

    cout << "\nState-aware execution finished.\n";
}