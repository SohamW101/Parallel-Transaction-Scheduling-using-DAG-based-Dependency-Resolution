#include "Metrics.h"
using namespace std;

Metrics::Metrics() {
    logFile.open("metrics.log", ios::out);
    if (!logFile.is_open()) {
        cout << "Failed to open metrics.log\n";
    }
}

Metrics::~Metrics() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Metrics::startGlobalTimer() {
    globalStart = chrono::high_resolution_clock::now();
}

long long Metrics::getElapsedMs() const {
    auto now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(now - globalStart).count();
}

long long Metrics::getElapsedUs() const {
    auto now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(now - globalStart).count();
}

long long Metrics::measureDuration(function<void()> func) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

void Metrics::log(const string &msg) {
    if (logFile.is_open()) {
        logFile << msg << endl;
    }
}
