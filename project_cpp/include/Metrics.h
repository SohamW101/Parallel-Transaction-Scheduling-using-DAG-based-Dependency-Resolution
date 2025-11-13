#ifndef METRICS_H
#define METRICS_H

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <functional>
using namespace std;

class Metrics {
private:
    chrono::high_resolution_clock::time_point globalStart;
    ofstream logFile;

public:
    Metrics();
    ~Metrics();

    void startGlobalTimer();
    long long getElapsedMs() const;   // milliseconds
    long long getElapsedUs() const;   // microseconds

    // measureDuration accepts a callable and returns elapsed ms
    long long measureDuration(function<void()> func);

    void log(const string &msg);
};

#endif // METRICS_H
