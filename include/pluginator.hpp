#pragma once
#include <string>
#include <vector>
#include <map>
#include "logger.hpp"
#include "plugin.hpp"

using namespace std;

class Pluginator {
    string prodServerPath;
    string testServerPath;
    string backupDir;
    string pluginVersionsFile;
    int maxBackups;

    Logger logger;
    vector<Plugin> plugins;
    map<string, string> pluginVersions;
    
    void initPlugins();

public:
    Pluginator();

    // backup functions
    void backup(const string& serverPath, const string& backupName);
    void viewRecentBackups();

    // bulk jar functions
    void sync();
    void migrateToProduction();

    // purpur functions
    void checkPurpur();
    void downloadPurpur(const string& version);

    // plugin functions
    void initPluginVersions();
    string getStoredVersion(const string& pluginName);
    void setStoredVersion(const string& pluginName, const string& version);
    string checkSpigotVersion(const string& resourceId);
    string checkModrinthVersion(const string& projectSlug, const string& mcVersion);
    void checkPluginUpdates(const string& mode); // mode -> "check" or "update"
    void downloadPluginUpdate(const string& pluginRef, const string& versionInfo);
    void showPluginStatus();

    // cli functions
    void showMenu();
    void runInteractive();

    // utils
    void disablePluginsForTest();
    string getCurrentTimestamp();
};