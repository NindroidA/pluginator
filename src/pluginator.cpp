#include "../include/pluginator.hpp"
#include "../include/colors.hpp"
#include "version.hpp"
#include <iostream>
#include <filesystem>
#include <sstream>

using namespace std;
using namespace filesystem;

Pluginator::Pluginator() : logger() {
    // initialize paths (THESE ARE CURRENTLY JUST TESTING PATHS)
    prodServerPath = "./prod";
    testServerPath = "./test";
    backupDir = "./backups";

    // create backup dir if it doesn't exist
    create_directories(backupDir);

    // load and initialize plugins
    initPluginVersions();
    initPlugins();

    logger.log("Pluginator " + string(PLUGINATOR_VERSION) + " initialized!");
    logger.log("Production: " + prodServerPath);
    logger.log("Test: " + testServerPath);
    logger.log("Backups: " + backupDir);
}

/**
 * getCurrentTimestamp
 * @brief function to get the current timestamp
 * @return string of the current timestamp
 */
string Pluginator::getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

/**
 * initPlugins
 * @brief function to initialize all listed plugins
 */
void Pluginator::initPlugins() {}

/**
 * viewRecentBackups
 * @brief function to view the recently made backups
 */
void Pluginator::viewRecentBackups() {}

/**
 * checkPurpur
 * @brief function to check for purpur updates
 */
void Pluginator::checkPurpur() {}

/**
 * downloadPurpur
 * @brief function to actually download the purpur jar
 * @param version string reference to the version of purpur wanted
 */
void Pluginator::downloadPurpur(const string& version) {};

/**
 * initPluginVersions
 * @brief function to initialize plugin versions file (if it doesn't already exist)
 */
void Pluginator::initPluginVersions() {}

/**
 * getStoredVersion
 * @brief function to get the version of a plugin from the stored plugin versions list
 * @param pluginName string reference to the name of the plugin
 * @return string of what version is currently installed for the given plugin
 */
string Pluginator::getStoredVersion(const string& pluginName) {}

/**
 * setStoredVersion
 * @brief function to update the version of a plugin from the stored plugin versions list
 * @param pluginName string reference to the name of the plugin
 * @param version string reference to the updated version of the plugin
 */
void Pluginator::setStoredVersion(const string& pluginName, const string& version) {}

/**
 * checkSpigotVersion
 * @brief function to check the version of a spigot plugin through the Spiget API
 * @param resourceId string reference to the resourceId of the resource actually on spigotmc
 * @return string of the latest version
 */
string Pluginator::checkSpigotVersion(const string& resourceId) {};

/**
 * checkModrinthVersion
 * @brief function to check the version of a modrinth plugin through the Modrinth API
 * @param projectSlug string reference to the slug of the project actually on modrinth
 * @param mcVersion string reference to the minecraft version the server is on (to download the correct plugin version)
 * @return string of the latest compatible version
 */
string Pluginator::checkModrinthVersion(const string& projectSlug, const string& mcVersion) {}

/**
 * checkPluginUpdates
 * @brief function to check all initialized plugins for updates
 * @param mode string reference to the mode (either "check" or "update")
 */
void Pluginator::checkPluginUpdates(const string& mode) {}

/**
 * downloadPluginUpdate
 * @brief function to actually download the latest version (or compatible version) of a given plugin
 * @param pluginRef string reference to the plugin that will be updated
 * @param versionInfo string reference to the versions a plugin has available
 * 
 */
void Pluginator::downloadPluginUpdate(const string& pluginRef, const string& versionInfo) {}

/**
 * showPluginStatus
 * @brief function to show the current status of all initialized plugins
 */
void Pluginator::showPluginStatus() {}

/**
 * showMenu
 * @brief function to show the cli menu
 */
void Pluginator::showMenu() {}

/**
 * runInteractive
 * @brief function to handle the cli menu interactions
 */
void Pluginator::runInteractive() {}


/**
 * sync
 * @brief function to sync plugins from production server to test server
 */
void Pluginator::sync() {
    logger.log("🔄 Syncing plugins from Production → Test...");

    string prodPluginsPath = prodServerPath + "/plugins";
    string testPluginsPath = testServerPath + "/plugins";

    // confirm plugins dir exists for both servers
    if (!exists(prodPluginsPath)) {
        logger.err("Error: Production Server plugins directory not found: " + prodPluginsPath);
        return;
    }

    if (!exists(testPluginsPath)) {
        logger.err("Error: Test Server plugins directory not found: " + testPluginsPath);
        return;
    }

    // create backup of test server plugins
    backup(testPluginsPath, "test_plugins_before_sync");
    
    // remove old jar files from test server
    for (const auto& entry : directory_iterator(testPluginsPath)) {
        if (entry.path().extension() == ".jar") {
            remove(entry.path());
            cout << "Removed: " << entry.path().filename() << endl;
        }
    }

    // copy jar files from production server to test server
    int copiedCount = 0;
    for (const auto& entry : directory_iterator(prodPluginsPath)) {
        if (entry.path().extension() == ".jar") {
            path destination = path(testPluginsPath) / entry.path().filename();
            copy_file(entry.path(), destination);
            cout << "Copied: " << entry.path().filename() << endl;
            copiedCount++;
        }
    }

    logger.success("✅ Sync completed! Copied " + to_string(copiedCount) + " plugins");

    // disable plugins that can't be running on the test server
    disablePluginsForTest();
}

/**
 * migrateToProduction
 * @brief function to migrate Test Server plugins to Production Server
 */
void Pluginator::migrateToProduction() {}

void Pluginator::disablePluginsForTest() {
    cout << "🚫 Disabling selected plugins for Test Server..." << endl;

    string testPluginsPath = testServerPath + "/plugins";
    int disabledCount = 0;

    for (const Plugin& plugin : plugins) {
        if (plugin.disableOnTest) {
            // find the matching files and add .DIS extension
            for (const auto& entry : directory_iterator(testPluginsPath)) {
                string filename = entry.path().filename().string();

                // pattern matching
                if (filename.find(plugin.name.substr(0, 3)) != string::npos && 
                    (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".jar")) {
                    
                    path newPath = entry.path();
                    newPath += ".DIS";
                    rename(entry.path(), newPath);
                    cout << "Disabled: " << filename << " → " << newPath.filename() << endl;
                    disabledCount++;
                }
            }
        }
    }

    cout << "✅ Disabled " << disabledCount << " plugins for Test Server" << endl;
}

/**
 * backup
 * @brief function to create a backup (and clean up the older ones)
 * @param serverPath string reference to the server path wanted to be backed up
 * @param backupName string reference to the name of the backup
 */
void Pluginator::backup(const string& serverPath, const string& backupName) {
    /* @todo will need to clean this up since this is all just me testing/learning tings*/
    cout << "💾 Creating backup: " << backupName << "..." << endl;

    string timestamp = getCurrentTimestamp();
    string backupFile = backupDir + "/" + backupName + "_" + timestamp + ".tar.gz";
    string command = "tar -czf \"" + backupFile + "\" -C \"" + serverPath + "\" .";

    int result = system(command.c_str());
    if (result == 0) {
        cout << "✅ Backup created: " << backupFile << endl;
    } else {
        cerr << "❌ Failed to create backup!" << endl;
    }

}