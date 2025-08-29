#include "../include/logger.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace filesystem;

Logger::Logger(const string& logDirectory, int maxDays) : logsDir(logDirectory), maxLogDays(maxDays) {
    initLogging();
    cleanupOldLogs();
}

/**
 * getCurrentLogFile
 * @brief function to get the current log file path
 * @return string of the log file path
 */
string Logger::getCurrentLogFile() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&time_t), "%Y-%m-%d");
    return logsDir + "/pluginator_" + ss.str() + ".log";
}

/**
 * initLogging
 * @brief function to initialize log directory and file
 */
void Logger::initLogging() {
    create_directories(logsDir);
    string currentLog = getCurrentLogFile();

    // create log file if it doesn't exist
    if (!exists(currentLog)) {
        ofstream file(currentLog);
        if (file.is_open()) {
            auto now = chrono::system_clock::now();
            auto time_t = chrono::system_clock::to_time_t(now);
            file << "=== Pluginator Log Started: "
                 << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                 << " ===" << endl;
            file.close();
        }
    }
}

/**
 * logToFile
 * @brief logging function that writes to daily log files with a timestamp
 * @param level string reference to the "level" of the log (log, warn, err, etc)
 * @param msg string reference to the message wanted to be logged
 */
void Logger::logToFile(const string& level, const string& msg) {
    string currentLog = getCurrentLogFile();
    create_directories(logsDir);
    
    ofstream file(currentLog, ios::app);
    if (file.is_open()) {
        file << "[" << getCurrentTimestamp() << "] [" << level << "] " << msg << endl;
        file.close();
    }
}

/**
 * getCurrentTimestamp
 * @brief function to get the current timestamp
 * @return string of the current timestamp
 */
string Logger::getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * cleanupOldLogs
 * @brief function to clean up old log files (keeps last 30 days)
 */
void Logger::cleanupOldLogs() {
    if (!exists(logsDir)) return;
    
    auto cutoffTime = chrono::system_clock::now() - chrono::hours(24 * maxLogDays);
    auto cutoffTimeT = chrono::system_clock::to_time_t(cutoffTime);
    int deletedCount = 0;
    
    for (const auto& entry : directory_iterator(logsDir)) {
        if (entry.path().extension() == ".log") {
            auto fileTime = fileTimeToTimeT(entry.last_write_time());
        
            if (fileTime < cutoffTimeT) {
                try {
                    remove(entry.path());
                    deletedCount++;

                } catch (const exception& e) {}
            }
        }
    }
    
    if (deletedCount > 0) {
        log("Cleaned up " + to_string(deletedCount) + " old log files (older than " + 
            to_string(maxLogDays) + " days)");
    }
}

/**
 * viewRecentLogs
 * @brief function to view most recent logs
 */
void Logger::viewRecentLogs() {
    cout << "\n" << Colors::BLUE << "📋 Recent Log Files:" << Colors::NC << endl;
    
    if (!exists(logsDir)) {
        cout << "Logs directory not found: " << logsDir << endl;
        return;
    }
    
    cout << "Available log files:" << endl;
    
    // explicit type declaration (cause it was being a lil bish)
    std::vector<std::pair<filesystem::path, time_t>> logFiles;
    
    for (const auto& entry : directory_iterator(logsDir)) {
        string filename = entry.path().filename().string();
        if (filename.find("pluginator_") == 0 && entry.path().extension() == ".log") {
            auto fileTimeT = fileTimeToTimeT(entry.last_write_time());
            logFiles.emplace_back(entry.path(), fileTimeT);
        }
    }
    
    // sort by modification time 
    sort(logFiles.begin(), logFiles.end(), 
         [](const std::pair<filesystem::path, time_t>& a, const std::pair<filesystem::path, time_t>& b) {
             return a.second > b.second;
         });
    
    // show last 10 log files
    int count = 0;
    for (const std::pair<filesystem::path, time_t>& logEntry : logFiles) {
        if (count++ >= 10) break;
        
        time_t fileTime = logEntry.second;
        char timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&fileTime));
        
        cout << "  " << logEntry.first.filename().string()
             << " - " << timeStr << endl;
    }
    
    cout << "\nLatest log entries:" << endl;
    string currentLog = getCurrentLogFile();
    if (exists(currentLog)) {
        ifstream file(currentLog);
        if (file.is_open()) {
            vector<string> lines;
            string line;
            while (getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
            
            int startLine = max(0, (int)lines.size() - 20);
            for (size_t i = startLine; i < lines.size(); i++) {
                cout << lines[i] << endl;
            }
        }
    } else {
        cout << "No entries in today's log yet." << endl;
    }
    cout << endl;
}

void Logger::enableDebug(bool enabled) { debugMode = enabled; }
bool Logger::isDebugEnabled() const { return debugMode; }


// ========= LOGGING FUNCTIONS ========== //
/* debug */
void Logger::debug(const string& msg) {
    if (debugMode) {
        cout << Colors::NC << "[DEBUG] 🐛" << msg << Colors::NC << endl;
        logToFile("DEBUG", msg);
    }
}

/* normal log */
void Logger::log(const string& msg) {
    cout << "[LOG] 📋" << msg << endl;
    logToFile("LOG", msg);
}

/* logging a success */
void Logger::success(const string& msg) {
    cout << Colors::GREEN << "[SUCCESS] ✅" << msg << Colors::NC << endl;
    logToFile("SUCCESS", msg);
}

/* logging a warning */
void Logger::warn(const string& msg) {
    cout << Colors::YELLOW << "[WARNING] ⚠️" << msg << Colors::NC << endl;
    logToFile("WARNING", msg);
}

/* logging an error */
void Logger::err(const string& msg) {
    cout << Colors::RED << "[ERROR] ❌" << msg << Colors::NC << endl;
    logToFile("ERROR", msg);
}

// @todo make a debug level

/* prompt for the user to input */
string Logger::prompt(const string& msg) {
    cout << "[PROMPT] " << msg;
    string input;
    getline(cin, input);
    return input;
}