// TraceWriter.cpp
#include "TraceWriter.h"
#include <fstream>

TraceWriter& TraceWriter::get() {
    static TraceWriter inst;
    return inst;
}

void TraceWriter::pushEvent(const std::string &jsonEvent) {
    std::lock_guard<std::mutex> lg(m);
    events.push_back(jsonEvent);
}

void TraceWriter::flushToFile(const std::string &path) {
    std::lock_guard<std::mutex> lg(m);
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out.is_open()) return;
    out << "[\n";
    for (size_t i = 0; i < events.size(); ++i) {
        out << events[i];
        if (i + 1 < events.size()) out << ",\n";
        else out << "\n";
    }
    out << "]\n";
    out.close();
}
