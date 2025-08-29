#pragma once
#include "http_client.hpp"
#include "logger.hpp"
#include <string>

using namespace std;

struct PurpurInfo {
    string latestVersion;
    string latestBuild;
    string buildResult;
    string downloadUrl;
    bool hasUpdate;
    string error;
    
    PurpurInfo() : hasUpdate(false) {}
};

class PurpurManager {
private:
    HttpClient httpClient;
    Logger& logger;
    
public:
    PurpurManager(Logger& log);
    
    // Purpur operations
    PurpurInfo checkPurpurUpdate();
    bool downloadPurpur(const string& version, const string& outPath);
    void showPurpurStatus();
};