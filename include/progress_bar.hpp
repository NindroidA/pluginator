#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace std;

class ProgressBar {
private:
    int total;
    int current;
    int width;
    string label;
    string currentItem;
    chrono::steady_clock::time_point startTime;
    bool isFinished;
    
    void renderBar() const;
    string formatTime(double seconds) const;
    string formatRate(double itemsPerSecond) const;
    string getProgressColor(double progress) const;

public:
    ProgressBar(int total, const string& label = "", int width = 50);
    
    // core functionality
    void update(int newCurrent, const string& itemName = "");
    void increment(const string& itemName = "");
    void setLabel(const string& newLabel);
    void finish(const string& completionMessage = "");
    
    // status methods
    bool isComplete() const;
    double getProgress() const;
    double getElapsedSeconds() const;
    
    // reset for reuse
    void reset(int newTotal = -1, const string& newLabel = "");
    
    // disable copy constructor and assignment
    ProgressBar(const ProgressBar&) = delete;
    ProgressBar& operator=(const ProgressBar&) = delete;
};