#include "../include/logger.hpp"
#include "../include/colors.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;
using namespace filesystem;

Logger::Logger(const string& logDirectory, int maxDays) : logsDir(logDirectory), maxLogDays(maxDays) {}

/**
 * getCurrentLogFile
 * @brief function to get the current log file path
 * @return string of the log file path
 */
string Logger::getCurrentLogFile() {}

/**
 * initLogging
 * @brief function to initialize log directory and file
 */
void Logger::initLogging() {}

/**
 * getCurrentTimestamp
 * @brief function to get the current timestamp
 * @return string of the current timestamp
 */
string Logger::getCurrentTimestamp() {}

/**
 * logToFile
 * @brief logging function that writes to daily log files
 */
void Logger::logToFile(const string& level, const string& msg) {}

/**
 * cleanupOldLogs
 * @brief function to clean up old log files (keeps last 30 days)
 */
void Logger::cleanupOldLogs() {}

/**
 * viewRecentLogs
 * @brief function to view most recent logs
 */
void Logger::viewRecentLogs() {}


// ========= LOGGING FUNCTIONS ========== //
/* normal log */
void Logger::log(const string& msg) {}

/* logging a success */
void Logger::success(const string& msg) {}

/* logging a warning */
void Logger::warn(const string& msg) {}

/* logging an error */
void Logger::err(const string& msg) {}

/* prompt for the user to input */
string Logger::prompt(const string& msg) {}