#include "../include/progress_bar.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

ProgressBar::ProgressBar(int total, const std::string& label, int width) 
    : total(total), current(0), width(width), label(label), isFinished(false) {
    startTime = std::chrono::steady_clock::now();
}

void ProgressBar::update(int newCurrent, const std::string& itemName) {
    if (isFinished) return;
    
    current = std::min(newCurrent, total);
    currentItem = itemName;
    renderBar();
    
    if (current >= total) {
        isFinished = true;
    }
}

void ProgressBar::increment(const std::string& itemName) {
    update(current + 1, itemName);
}

void ProgressBar::setLabel(const std::string& newLabel) {
    label = newLabel;
}

void ProgressBar::renderBar() const {
    double progress = total > 0 ? (double)current / total * 100.0 : 0.0;
    int filled = total > 0 ? (int)(width * current / total) : 0;
    
    // clear the entire line first
    std::cout << "\r\033[K";
    
    // progress bar
    std::cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            std::cout << "=";
        } else if (i == filled && current < total) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] ";
    
    // percentage
    std::cout << std::fixed << std::setprecision(1) << progress << "% ";
    
    // count
    std::cout << "(" << current << "/" << total << ")";
    
    // label
    if (!label.empty()) {
        std::cout << " " << label;
    }
    
    // current item (truncate if too long)
    if (!currentItem.empty()) {
        std::string displayItem = currentItem;
        const int maxItemLength = 25;
        if (displayItem.length() > maxItemLength) {
            displayItem = displayItem.substr(0, maxItemLength - 3) + "...";
        }
        std::cout << " [" << displayItem << "]";
    }
    
    // time info
    double elapsed = getElapsedSeconds();
    if (elapsed > 1.0 && current > 0) {
        double rate = current / elapsed;
        std::cout << " " << formatRate(rate);
        
        if (current < total && rate > 0) {
            double eta = (total - current) / rate;
            std::cout << " ETA: " << formatTime(eta);
        }
    }
    
    std::cout << std::flush;
}

void ProgressBar::finish(const std::string& completionMessage) {
    if (!isFinished) {
        current = total;
        renderBar();
        isFinished = true;
    }
    
    std::cout << std::endl;
    
    if (!completionMessage.empty()) {
        std::cout << completionMessage << std::endl;
    } else {
        double elapsed = getElapsedSeconds();
        std::cout << "Complete! (" << formatTime(elapsed) << ")" << std::endl;
    }
    
    std::cout << std::endl;
}

bool ProgressBar::isComplete() const {
    return isFinished || current >= total;
}

double ProgressBar::getProgress() const {
    return total > 0 ? (double)current / total : 0.0;
}

double ProgressBar::getElapsedSeconds() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return duration.count() / 1000.0;
}

void ProgressBar::reset(int newTotal, const std::string& newLabel) {
    if (newTotal > 0) {
        total = newTotal;
    }
    if (!newLabel.empty()) {
        label = newLabel;
    }
    
    current = 0;
    currentItem.clear();
    isFinished = false;
    startTime = std::chrono::steady_clock::now();
}

std::string ProgressBar::formatTime(double seconds) const {
    if (seconds < 60) {
        return std::to_string((int)seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = (int)(seconds / 60);
        int secs = (int)seconds % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = (int)(seconds / 3600);
        int minutes = ((int)seconds % 3600) / 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

std::string ProgressBar::formatRate(double itemsPerSecond) const {
    if (itemsPerSecond >= 1.0) {
        return "(" + std::to_string((int)itemsPerSecond) + "/s)";
    } else {
        double secondsPerItem = 1.0 / itemsPerSecond;
        return "(" + formatTime(secondsPerItem) + "/item)";
    }
}