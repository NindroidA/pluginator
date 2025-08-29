#pragma once
#include <string>

using namespace std;

class Logger {
    string logsDir;
    int maxLogDays;
    bool debugMode = false;
    string getCurrentLogFile();
    void initLogging();
    void logToFile(const string& level, const string& msg);
    string getCurrentTimestamp();

public:
    Logger(const string& logDirectory = "./logs", int maxDays = 30);

    // log management functions
    void cleanupOldLogs();
    void viewRecentLogs();

    // debug utils
    void enableDebug(bool enabled);
    bool isDebugEnabled() const;

    // core logging functions
    void debug(const string& msg);
    void log(const string& msg);
    void success(const string& msg);
    void warn(const string& msg);
    void err(const string& msg);
    string prompt(const string& msg);
};