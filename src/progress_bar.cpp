#include "../include/progress_bar.hpp"
#include "../include/colors.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

ProgressBar::ProgressBar(int total, const string& label, int width) 
    : total(total), current(0), width(width), label(label), isFinished(false) {
    startTime = chrono::steady_clock::now();
}

void ProgressBar::update(int newCurrent, const string& itemName) {
    if (isFinished) return;
    
    current = min(newCurrent, total);
    currentItem = itemName;
    renderBar();
    
    if (current >= total) {
        isFinished = true;
    }
}

void ProgressBar::increment(const string& itemName) {
    update(current + 1, itemName);
}

void ProgressBar::setLabel(const string& newLabel) {
    label = newLabel;
}

void ProgressBar::renderBar() const {
    double progress = total > 0 ? (double)current / total * 100.0 : 0.0;
    int filled = total > 0 ? (int)(width * current / total) : 0;
    
    // clear the entire line first
    cout << "\r\033[K";
    
    // progress bar
    cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            cout << Colors::PURPLE << "=";
        } else if (i == filled && current < total) {
            cout << Colors::PURPLE << ">";
        } else {
            cout << " ";
        }
    }
    cout << Colors::NC << "] ";
    
    // percentage
    cout << fixed << setprecision(1) << Colors::BRIGHT_WHITE << progress << "% " << Colors::NC << " ";
    
    // count
    string progressColor = getProgressColor(progress);
    cout << progressColor << "(" << current << "/" << total << ")" << Colors::NC << " ";
    
    // label
    if (!label.empty()) {
        cout << " " << Colors::DIM_WHITE << label << Colors::NC;
    }
    
    // current item (truncate if too long)
    if (!currentItem.empty()) {
        string displayItem = currentItem;
        const int maxItemLength = 25;
        if (displayItem.length() > maxItemLength) {
            displayItem = displayItem.substr(0, maxItemLength - 3) + "...";
        }
        cout << " " << Colors::CRIMSON << " [" << displayItem << "]" << Colors::NC << " ";
    }
    
    // time info
    double elapsed = getElapsedSeconds();
    if (elapsed > 1.0 && current > 0) {
        double rate = current / elapsed;
        cout << " " << Colors::SOFT_GREEN << formatRate(rate) << Colors::NC << " ";
        
        if (current < total && rate > 0) {
            double eta = (total - current) / rate;
            cout << Colors::BRIGHT_BLUE << " ETA: " << formatTime(eta) << Colors::NC << " ";
        }
    }
    
    cout << flush;
}

void ProgressBar::finish(const string& completionMessage) {
    if (!isFinished) {
        current = total;
        renderBar();
        isFinished = true;
    }
    
    cout << endl;
    
    if (!completionMessage.empty()) {
        cout << completionMessage << endl;
    } else {
        double elapsed = getElapsedSeconds();
        cout << "Complete! (" << formatTime(elapsed) << ")" << endl;
    }
    
    cout << endl;
}

bool ProgressBar::isComplete() const {
    return isFinished || current >= total;
}

double ProgressBar::getProgress() const {
    return total > 0 ? (double)current / total : 0.0;
}

double ProgressBar::getElapsedSeconds() const {
    auto now = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(now - startTime);
    return duration.count() / 1000.0;
}

void ProgressBar::reset(int newTotal, const string& newLabel) {
    if (newTotal > 0) {
        total = newTotal;
    }
    if (!newLabel.empty()) {
        label = newLabel;
    }
    
    current = 0;
    currentItem.clear();
    isFinished = false;
    startTime = chrono::steady_clock::now();
}

string ProgressBar::formatTime(double seconds) const {
    if (seconds < 60) {
        return to_string((int)seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = (int)(seconds / 60);
        int secs = (int)seconds % 60;
        return to_string(minutes) + "m " + to_string(secs) + "s";
    } else {
        int hours = (int)(seconds / 3600);
        int minutes = ((int)seconds % 3600) / 60;
        return to_string(hours) + "h " + to_string(minutes) + "m";
    }
}

string ProgressBar::formatRate(double itemsPerSecond) const {
    if (itemsPerSecond >= 1.0) {
        return "(" + to_string((int)itemsPerSecond) + "/s)";
    } else {
        double secondsPerItem = 1.0 / itemsPerSecond;
        return "(" + formatTime(secondsPerItem) + "/item)";
    }
}

string ProgressBar::getProgressColor(double progress) const {
    if (progress < 25.0) return Colors::RED;
    else if (progress < 50.0) return Colors::ORANGE;
    else if (progress < 75.0) return Colors::YELLOW;
    else return Colors::GREEN;
}