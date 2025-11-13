#include "DAGExporter.h"
#include <fstream>
#include <iostream>
using namespace std;

void DAGExporter::exportToDOT(const DAG &dag, const string &filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Failed to open DOT file: " << filename << endl;
        return;
    }

    out << "digraph DAG {\n";

    auto adj = dag.getAdjList();
    auto nodes = dag.getAllNodes();

    for (auto &n : nodes)
        out << "    \"" << n << "\";\n";

    for (auto &p : adj)
        for (auto &to : p.second)
            out << "    \"" << p.first << "\" -> \"" << to << "\";\n";

    out << "}\n";
    out.close();

    cout << "Exported DAG to DOT file: " << filename << endl;
}

void DAGExporter::exportToJSON(const DAG &dag, const string &filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Failed to open JSON file: " << filename << endl;
        return;
    }

    auto adj = dag.getAdjList();
    auto nodes = dag.getAllNodes();

    out << "{\n";

    for (size_t i = 0; i < nodes.size(); i++) {
        const string &node = nodes[i];
        out << "  \"" << node << "\": [";

        if (adj.count(node)) {
            const auto &vec = adj.at(node);
            for (size_t j = 0; j < vec.size(); j++) {
                out << "\"" << vec[j] << "\"";
                if (j + 1 < vec.size()) out << ", ";
            }
        }

        out << "]";
        if (i + 1 < nodes.size()) out << ",";
        out << "\n";
    }

    out << "}\n";
    out.close();

    cout << "Exported DAG to JSON file: " << filename << endl;
}