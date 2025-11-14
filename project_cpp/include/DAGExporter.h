// DAGExporter.h
#ifndef DAGEXPORTER_H
#define DAGEXPORTER_H

#include "DAG.h"
#include <string>

class DAGExporter {
public:
    static void exportToDOT(const DAG &dag, const std::string &filename);
    static void exportToJSON(const DAG &dag, const std::string &filename);
};

#endif
