#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

class ProgressBar {
private:
    int total;
    int current;
    int width;
    std::string label;
    std::string currentItem;
    std::chrono::steady_clock::time_point startTime;
    bool isFinished;
    
    void renderBar() const;
    std::string formatTime(double seconds) const;
    std::string formatRate(double itemsPerSecond) const;

public:
    ProgressBar(int total, const std::string& label = "", int width = 50);
    
    // core functionality
    void update(int newCurrent, const std::string& itemName = "");
    void increment(const std::string& itemName = "");
    void setLabel(const std::string& newLabel);
    void finish(const std::string& completionMessage = "");
    
    // status methods
    bool isComplete() const;
    double getProgress() const;
    double getElapsedSeconds() const;
    
    // reset for reuse
    void reset(int newTotal = -1, const std::string& newLabel = "");
    
    // disable copy constructor and assignment
    ProgressBar(const ProgressBar&) = delete;
    ProgressBar& operator=(const ProgressBar&) = delete;
};