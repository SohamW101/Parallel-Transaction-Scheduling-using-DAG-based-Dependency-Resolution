// TraceWriter.h
#pragma once
#include <string>
#include <vector>
#include <mutex>

class TraceWriter {
public:
    static TraceWriter& get();

    // Add a JSON-formatted event string (already escaped)
    void pushEvent(const std::string &jsonEvent);

    // Write collected events into a file path as a JSON array
    void flushToFile(const std::string &path);

private:
    TraceWriter() = default;
    std::vector<std::string> events;
    std::mutex m;
};
