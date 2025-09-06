#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include "logger.hpp"
#include "plugin.hpp"
#include "plugin_manager.hpp"
#include "purpur_manager.hpp"

using namespace std;

class Pluginator {
    string prodServerPath;
    string testServerPath;
    string backupDir;
    string pluginVersionsFile;
    int maxBackups;

    Logger logger;
    vector<Plugin> plugins;
    
    PluginManager pluginManager;
    PurpurManager purpurManager;
    
    void initPlugins();
    void backup(const string& serverPath, const string& backupName);
    void cleanupOldBackups(const string& backupName);
    void enablePluginsForTest();

public:
    Pluginator();

    // backup functions
    void backupProduction();
    void backupTest();
    void viewRecentBackups();
    void viewRecentLogs();

    // bulk jar functions
    void sync();
    void migrateToProduction();

    // purpur functions
    void checkPurpur();
    void downloadPurpur();

    // plugin functions
    void checkPluginUpdates();
    void downloadPluginUpdates();
    void showPluginUpdates();
    void scanAndUpdateVersions();
    void scanVersionsAPI();
    void verifyVersions();

    // cli functions
    void showMenu();
    void runInteractive();
    void showConfiguration();

    // utils
    void disablePluginsForTest();
    string getCurrentTimestamp();
    //const vector<Plugin>& getLoadedPlugins();
};