#include "DAGExporter.h"
#include <fstream>
#include <iostream>

void DAGExporter::exportToDOT(const DAG &dag, const std::string &filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    auto adj = dag.getAdjList();

    out << "digraph G {\n";
    for (auto &p : adj) {
        for (auto &to : p.second) {
            out << "    \"" << p.first << "\" -> \"" << to << "\";\n";
        }
    }
    out << "}\n";
}

void DAGExporter::exportToJSON(const DAG &dag, const std::string &filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    auto adj = dag.getAdjList();

    out << "{\n";

    // --- nodes ---
    out << "  \"nodes\": [\n";
    bool first = true;
    for (auto &p : adj) {
        if (!first) out << ",\n";
        first = false;
        out << "    {\"id\": \"" << p.first << "\"}";
    }
    out << "\n  ],\n";

    // --- edges ---
    out << "  \"edges\": [\n";
    bool firstEdge = true;
    for (auto &p : adj) {
        for (auto &to : p.second) {
            if (!firstEdge) out << ",\n";
            firstEdge = false;
            out << "    {\"from\": \"" << p.first << "\", \"to\": \"" << to << "\"}";
        }
    }
    out << "\n  ]\n";

    out << "}\n";
}
