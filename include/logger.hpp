#pragma once
#include <string>

using namespace std;

class Logger {
    // basic settings
    string logsDir;
    int maxLogDays;
    bool debugMode = false;
    size_t maxLogFileSize = 10 * 1024 * 1024; // 10MB default

    // internal methods
    string getCurrentLogFile();
    void initLogging();
    void logToFile(const string& level, const string& msg);
    string getCurrentTimestamp();

public:
    // constructor
    Logger(const string& logDirectory = "./logs", int maxDays = 30);

    // log management functions
    void cleanupOldLogs();
    void viewRecentLogs();

    // file management
    void checkLogFileSize();
    void rotateLogFile();

    // debug utils
    void enableDebug(bool enabled);
    bool isDebugEnabled() const;

    // core logging functions
    void debug(const string& msg);
    void log(const string& msg);
    void info(const string& msg);
    void success(const string& msg);
    void warn(const string& msg);
    void err(const string& msg);
    string prompt(const string& msg);
};