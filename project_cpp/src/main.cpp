// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Transaction.h"
#include "DAG.h"
#include "DAGExporter.h"
#include "Executor.h"
#include "State.h"
#include "Metrics.h"
#include "Utils.h"
#include "TraceWriter.h"

using namespace std;

static string toJsonStringArray(const unordered_set<string> &s) {
    ostringstream o;
    o << "[";
    bool first = true;
    for (const auto &x : s) {
        if (!first) o << ",";
        first = false;
        // escape quotes inside id just in case
        string esc;
        for (char c: x) {
            if (c == '"') esc += "\\\"";
            else esc += c;
        }
        o << "\"" << esc << "\"";
    }
    o << "]";
    return o.str();
}

// Serialize DAG + full tx metadata into a JSON file that GUI expects
static void exportAugmentedDagJSON(const DAG &dag, const vector<Transaction> &txs, const string &path) {
    // gather adjacency and nodes
    auto adj = dag.getAdjList();

    // build a map for quick tx lookup
    unordered_map<string, Transaction> lookup;
    for (const auto &t : txs) lookup[t.getId()] = t;

    ofstream out(path, ios::out | ios::trunc);
    if (!out.is_open()) {
        cerr << "Failed to open " << path << " for writing\n";
        return;
    }

    out << "{\n";

    // nodes: ensure we include all transactions (from txs vector) and include read/write arrays
    out << "  \"nodes\": [\n";
    bool firstNode = true;
    for (const auto &t : txs) {
        if (!firstNode) out << ",\n";
        firstNode = false;
        out << "    {\n";
        out << "      \"id\": \"" << t.getId() << "\",\n";

        // read array
        out << "      \"read\": " << toJsonStringArray(t.getReadSet()) << ",\n";
        // write array
        out << "      \"write\": " << toJsonStringArray(t.getWriteSet()) << "\n";
        out << "    }";
    }
    out << "\n  ],\n";

    // edges: iterate adjacency list (if some tx has no entry in adj, skip)
    out << "  \"edges\": [\n";
    bool firstEdge = true;
    for (const auto &p : adj) {
        const string &src = p.first;
        for (const auto &dst : p.second) {
            if (!firstEdge) out << ",\n";
            firstEdge = false;
            out << "    {\"from\": \"" << src << "\", \"to\": \"" << dst << "\"}";
        }
    }
    out << "\n  ]\n";

    out << "}\n";
    out.close();

    cout << "Wrote augmented DAG JSON to " << path << "\n";
}

int main(int argc, char** argv) {
    cout << "=== Parallel DAG Executor — with GUI-friendly export ===\n";

    // create sample transactions & build DAG
    auto txs = createSampleTransactions();
    DAG dag;
    dag.buildFromTransactions(txs);

    // Export DOT as before (optional)
    DAGExporter::exportToDOT(dag, "dag_output.dot");

    // Export an augmented JSON that contains node metadata (read/write) so GUI can show them
    exportAugmentedDagJSON(dag, txs, "dag_output.json");

    // Also export original format (if you want) - optional
    DAGExporter::exportToJSON(dag, "dag_output_raw.json");

    // Prepare state + executor + metrics
    State state = createInitialState();
    Executor executor;
    Metrics metrics;
    metrics.startGlobalTimer();

    // Small example observer (kept minimal)
    executor.observer.onBatchStart = [](int batchId, const vector<string> &batch) {
        // keep observer light-weight
    };

    // Run execution and produce trace.json (TraceWriter is used inside Executor instrumentation)
    executor.executeWithState(dag, txs, state, 4, metrics);

    cout << "\nTotal execution time: " << metrics.getElapsedMs() << " ms\n";
    state.display();

    // Ensure trace is written out for GUI playback
    TraceWriter::get().flushToFile("trace.json");
    cout << "Wrote trace.json and dag_output.json (augmented with read/write sets).\n";

    return 0;
}
