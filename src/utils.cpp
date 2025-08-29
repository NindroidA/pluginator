#include "../include/utils.hpp"
#include <sstream>
#include <iomanip>

namespace Utils {
    
    string formatBytes(int64_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unit = 0;
        double size = (double)bytes;
        
        while (size >= 1024.0 && unit < 4) {
            size /= 1024.0;
            unit++;
        }
        
        ostringstream oss;
        if (size < 10.0 && unit > 0) {
            oss << fixed << setprecision(1) << size << " " << units[unit];
        } else {
            oss << fixed << setprecision(0) << size << " " << units[unit];
        }
        
        return oss.str();
    }
    
    string formatSpeed(double bytesPerSec) {
        return formatBytes((int64_t)bytesPerSec) + "/s";
    }
    
    string formatDuration(double seconds) {
        ostringstream oss;
        
        if (seconds < 60) {
            oss << (int)seconds << "s";
        } else if (seconds < 3600) {  // less than 1 hour
            int mins = (int)seconds / 60;
            int secs = (int)seconds % 60;
            oss << mins << "m" << setfill('0') << setw(2) << secs << "s";
        } else {  // 1 hour or more
            int hours = (int)seconds / 3600;
            int mins = ((int)seconds % 3600) / 60;
            int secs = (int)seconds % 60;
            oss << hours << "h" << setfill('0') << setw(2) << mins << "m" << setw(2) << secs << "s";
        }
        
        return oss.str();
    }
    
    string createProgressBar(double progress, int width, char fillChar, char progressChar, char emptyChar) {
        ostringstream bar;
        int filledWidth = (int)(width * progress / 100.0);
        
        bar << "[";
        for (int i = 0; i < width; ++i) {
            if (i < filledWidth) {
                bar << fillChar;
            } else if (i == filledWidth && progress < 100.0 && progress > 0.0) {
                bar << progressChar;
            } else {
                bar << emptyChar;
            }
        }
        bar << "]";
        
        return bar.str();
    }
}