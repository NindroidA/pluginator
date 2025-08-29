#pragma once
#include <filesystem>
#include <chrono>
#include <string>
#include <cstdint>

using namespace std;

namespace Utils {
    // file size formats
    string formatBytes(int64_t bytes);
    string formatSpeed(double bytesPerSec);
    
    // time formatting
    string formatDuration(double seconds);
    
    // progress bar shite
    string createProgressBar(double progress, int width = 40, char fillChar = '=', char progressChar = '>', char emptyChar = ' ');
}

// ========== combatibility helpers ========== //
inline time_t fileTimeToTimeT(const filesystem::file_time_type& ftime) {
    auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
        ftime - filesystem::file_time_type::clock::now() + chrono::system_clock::now()
    );
    return chrono::system_clock::to_time_t(sctp);
}

inline bool endsWith(const string& str, const string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.substr(str.length() - suffix.length()) == suffix;
}