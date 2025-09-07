#include "../include/lang.hpp"
#include "../include/utils.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>

using namespace filesystem;
using namespace std;

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

    string getInstallDirectory() {
        // check current working directory first (for development)
        filesystem::path workingDir = filesystem::current_path();
        
        // check if current working directory is the project root
        if (filesystem::exists(workingDir / "CMakeLists.txt") && 
            filesystem::exists(workingDir / "include") &&
            filesystem::exists(workingDir / "src")) {
            return workingDir.string();
        }
        
        // check if we're running from build directory (go up one level)
        if (workingDir.filename() == "build") {
            filesystem::path parentDir = workingDir.parent_path();
            if (filesystem::exists(parentDir / "CMakeLists.txt") && 
                filesystem::exists(parentDir / "include") &&
                filesystem::exists(parentDir / "src")) {
                return parentDir.string();
            }
        }
        
        // production: check if running from /usr/local/bin
        char* exePath = realpath("/proc/self/exe", nullptr);
        if (exePath != nullptr) {
            string fullPath(exePath);
            free(exePath);
            
            size_t lastSlash = fullPath.find_last_of('/');
            if (lastSlash != string::npos) {
                string binDir = fullPath.substr(0, lastSlash);
                
                // /usr/local/bin/pluginator -> /opt/pluginator
                if (binDir == "/usr/local/bin") {
                    return "/opt/pluginator";
                }
                
                // walk up from executable path looking for project markers
                filesystem::path currentPath(binDir);
                for (int i = 0; i < 5; i++) {
                    if (filesystem::exists(currentPath / "CMakeLists.txt") && 
                        filesystem::exists(currentPath / "include") &&
                        filesystem::exists(currentPath / "src")) {
                        return currentPath.string();
                    }
                    
                    filesystem::path parentPath = currentPath.parent_path();
                    if (parentPath == currentPath) break;  // reached root
                    currentPath = parentPath;
                }
            }
        }
        
        // final fallback
        return "/opt/pluginator";
    }
    
    string getConfigPath(const string& filename) {
        return getInstallDirectory() + "/config/" + filename;
    }
    
    string getDataPath(const string& filename) {
        return getInstallDirectory() + "/data/" + filename;
    }
    
    string getLangPath(const string& filename) {
        return getInstallDirectory() + "/lang/" + filename;
    }
}