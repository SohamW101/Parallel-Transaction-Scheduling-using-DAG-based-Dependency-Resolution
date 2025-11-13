#ifndef DAG_EXPORTER_H
#define DAG_EXPORTER_H

#include <string>
#include "DAG.h"
using namespace std;

class DAGExporter {
public:
    static void exportToDOT(const DAG &dag, const string &filename);
    static void exportToJSON(const DAG &dag, const string &filename);
};

#endif // DAG_EXPORTER_H