#include "../include/pluginator.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include "../include/config.hpp"
#include "../include/lang.hpp"
#include "../include/plugin_manager.hpp"
#include "../include/purpur_manager.hpp"
#include "version.hpp"
#include <iostream>
#include <filesystem>
#include <sstream>

using namespace std;
using namespace filesystem;

Pluginator::Pluginator() : logger(Config::getInstance().getLogsDir(), Config::getInstance().getMaxLogDays()), pluginManager(logger), purpurManager(logger) {

    // check for debug mode
    if (getenv("PLUGINATOR_DEBUG")) {
        logger.enableDebug(true);
    }

    // get configuration
    Config& config = Config::getInstance();
    prodServerPath = config.getProdServerPath();
    testServerPath = config.getTestServerPath();
    backupDir = config.getBackupDir();
    pluginVersionsFile = config.getPluginVersionsFile();
    maxBackups = config.getMaxBackups();

    // create backup dir if it doesn't exist
    create_directories(backupDir);

    // load and initialize plugins
    initPlugins();
    pluginManager.initPluginVersions();

    logger.debug(LANG("app.session_started"));
    logger.debug("Production: " + prodServerPath);
    logger.debug("Test: " + testServerPath);
    logger.debug("Backups: " + backupDir);
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
void Pluginator::initPlugins() {
    // try to load from external config
    if (pluginManager.initFromConfig(Utils::getDataPath("plugins.json"))) {
        logger.success(LANG("plugin.config_loaded_external"));

        plugins = pluginManager.getLoadedPlugins();

        logger.debug("Populated " + to_string(plugins.size()) + " plugins in main class");
        return;
    }

    // if no config file, generate one from plugin list
    /*
    if (exists(Utils::getDataPath("plugin-list.txt"))) {
        logger.debug(LANG("plugin.generating_from_list"));
        pluginManager.generateConfigFromPluginList(Utils::getDataPath("plugin-list.txt"));
        plugins = pluginManager.getLoadedPlugins();
        return;
    }
    */

    // fallback (scan existing plugins dir)
    string testPluginsPath = testServerPath + "/plugins";
    if (exists(testPluginsPath)) {
        logger.debug(LANG("plugin.scanning_existing"));
        pluginManager.scanAndConfigurePlugins(testPluginsPath);
        plugins = pluginManager.getLoadedPlugins();
        return;
    }

    // last resort (show help msg)
    logger.warn(LANG("plugin.no_config_found"));
    logger.log(LANG("plugin.config_help"));
}

/**
 * backup
 * @brief function to create a backup (and clean up the older ones)
 * @param serverPath string reference to the server path wanted to be backed up
 * @param backupName string reference to the name of the backup
 */
void Pluginator::backup(const string& serverPath, const string& backupName) {
    logger.log("💾 Creating backup: " + backupName);

    // create backup dir if it doesn't exist
    create_directories(backupDir);

    // create timestamp and backup file name
    string timestamp = getCurrentTimestamp();
    string backupFile = backupDir + "/" + backupName + "_" + timestamp + ".tar.gz";

    // check if src path exists
    if (!exists(serverPath)) {
        logger.err("Source path does not exist: " + serverPath);
        return;
    }

    // create tar command
    string command = "tar -czf \"" + backupFile + "\" -C \"" +
                    path(serverPath).parent_path().string() + "\" \"" +
                    path(serverPath).filename().string() + "\"";

    logger.log("Running: " + command);
    int result = system(command.c_str());

    if (result == 0) {
        logger.success("✅ Backup created: " + backupFile);

        // clean up old backups
        cleanupOldBackups(backupName);
    } else {
        logger.err("Failed to create backup!");
        return;
    }
}

void Pluginator::backupProduction() {
    backup(prodServerPath, "prod_server_manual");
}

void Pluginator::backupTest() {
    backup(testServerPath, "test_server_manual");
}

/**
 * cleanupOldBackups
 * @brief function to clean up older not needed backups
 * @param backupName string reference to the file name of the backup
 */
void Pluginator::cleanupOldBackups(const string& backupName) {
    logger.log("Cleaning up old backups (keeping " + to_string(maxBackups) + " newest) ...");

    if (!exists(backupDir)) { return; }

    // collect all backup files for this
    vector<pair<path, time_t>> backupFiles;
    string pattern = backupName + "_";

    for (const auto& entry : directory_iterator(backupDir)) {
        string filename = entry.path().filename().string();
        if (filename.find(pattern) == 0 && endsWith(filename, ".tar.gz")) {
            auto fileTime = fileTimeToTimeT(entry.last_write_time());
            backupFiles.push_back({ entry.path(), fileTime });
        }
    }

    // sort by time
    sort(backupFiles.begin(), backupFiles.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    // remove old backups beyond maxBackups
    int removedCount = 0;
    for (int i = maxBackups; i < backupFiles.size(); i++) {
        try {
            remove(backupFiles[i].first);
            logger.log("Removed old backup: " + backupFiles[i].first.filename().string());
            removedCount++;
        } catch (const exception& e) {
            logger.warn("Failed to remove backup: " + backupFiles[i].first.string());
        }
    }

    if (removedCount > 0) {
        logger.log("Cleaned up " + to_string(removedCount) + " old backup files");
    }
}

/**
 * viewRecentBackups
 * @brief function to view the recently made backups
 */
void Pluginator::viewRecentBackups() {
    cout << "\n" << Colors::BLUE << "📋 Recent Backups:" << Colors::NC << endl;
    
    if (!exists(backupDir)) {
        cout << "Backup directory not found: " << backupDir << endl;
        return;
    }
    
    // collect all backup files
    vector<pair<path, time_t>> backupFiles;
    
    for (const auto& entry : directory_iterator(backupDir)) {
        if (entry.path().extension() == ".gz" && 
            entry.path().stem().extension() == ".tar") {
            auto fileTime = fileTimeToTimeT(entry.last_write_time());
            backupFiles.push_back({entry.path(), fileTime});
        }
    }
    
    if (backupFiles.empty()) {
        cout << "No backups found." << endl;
        return;
    }
    
    // sort by time
    sort(backupFiles.begin(), backupFiles.end(), 
         [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // show last 10 backups
    int count = 0;
    for (const auto& backup : backupFiles) {
        if (count++ >= 10) break;
        
        // get file size
        auto fileSize = file_size(backup.first);
        string sizeStr;
        if (fileSize > 1024 * 1024) {
            sizeStr = to_string(fileSize / (1024 * 1024)) + "M";
        } else if (fileSize > 1024) {
            sizeStr = to_string(fileSize / 1024) + "K";
        } else {
            sizeStr = to_string(fileSize) + "B";
        }
        
        // format time
        auto time_t = backup.second;
        char timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&time_t));
        
        printf("  %-40s %8s  %s\n", 
               backup.first.filename().string().c_str(),
               sizeStr.c_str(),
               timeStr);
    }
    cout << endl;
}

void Pluginator::checkPluginUpdates() {
    pluginManager.checkAllPluginUpdates(plugins, false);
}

void Pluginator::downloadPluginUpdates() {
    pluginManager.checkAllPluginUpdates(plugins, true);
}

void Pluginator::showPluginUpdates() {
    pluginManager.showPluginStatus(plugins);
}

void Pluginator::scanAndUpdateVersions() {
    string pluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
    pluginManager.scanAndUpdateVersions(pluginsPath);
}

void Pluginator::scanVersionsAPI() {
    string pluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
    pluginManager.scanAndUpdateVersionsWithAPI(pluginsPath);
}

void Pluginator::verifyVersions() {
    string pluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
    pluginManager.verifyVersionsWithAPI(pluginsPath);
}

/**
 * checkPurpur
 * @brief function to check for purpur updates
 */
void Pluginator::checkPurpur() {
    purpurManager.showPurpurStatus();

    // ask if want to download latest
    cout << endl;
    string res = logger.prompt(LANG("purpur.download_prompt"));

    if (res == "y" || res == "Y" || res == "yes") {
        downloadPurpur();
    } else {
        logger.log(LANG("purpur.download_cancelled"));
    }
}

/**
 * downloadPurpur
 * @brief function to actually download the latest purpur jar
 */
void Pluginator::downloadPurpur() {
    PurpurInfo info = purpurManager.checkPurpurUpdate();
    
    if (!info.error.empty()) {
        logger.err(info.error);
        return;
    }
    
    // download to test server by default
    string filename = "purpur-" + info.latestVersion + "-" + info.latestBuild + ".jar";
    string outPath = testServerPath + "/" + filename;
    
    if (purpurManager.downloadPurpur(info.latestVersion, outPath)) {
        logger.success(LANGF("purpur.downloaded_to", outPath));
    }
};

/**
 * showMenu
 * @brief function to show the cli menu
 */
void Pluginator::showMenu() {
    system("clear");
    
    cout << Colors::CYAN;
    cout << "╔══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                          PLUGINATOR                          ║" << endl;
    cout << "║                      Version - " << PLUGINATOR_VERSION << "                         ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════╝" << endl;
    cout << Colors::NC << endl;
    
    cout << Colors::PURPLE << LANG("menu.update_operations") << Colors::NC << endl;
    cout << "   1) 📦 Sync plugins (Prod → Test)" << endl;
    cout << "   2) 🎯 Check Purpur updates" << endl;
    cout << "   3) 🚀 Full test server update (plugins + Purpur)" << endl;
    cout << "   4) 🔍 Check plugin updates" << endl;
    cout << "   5) ⬇️  Update plugins (download & install)" << endl;
    cout << endl;
    
    cout << Colors::YELLOW << LANG("menu.backup_operations") << Colors::NC << endl;
    cout << "   6) 🏭 Backup production server" << endl;
    cout << "   7) 🧪 Backup test server" << endl;
    cout << endl;
    
    cout << Colors::GREEN << LANG("menu.deployment") << Colors::NC << endl;
    cout << "   8) 🚚 Deploy test → production (with backup)" << endl;
    cout << endl;
    
    cout << Colors::BLUE << LANG("menu.server_management") << Colors::NC << endl;
    cout << "   9) 📋 View recent logs" << endl;
    cout << "  10) 📋 View recent backups" << endl;
    cout << "  11) 🔧 Show Plugin Status" << endl;
    cout << endl;
    
    cout << Colors::RED << "   0) " << LANG("menu.exit") << Colors::NC << endl;
    cout << endl;
    
    cout << Colors::CYAN << "╭─────────────────────────────────────────────────────────────╮" << Colors::NC << endl;
    cout << Colors::CYAN << "│" << Colors::NC << LANG("menu.choose_option");
}

/**
 * runInteractive
 * @brief function to handle the cli menu interactions
 */
void Pluginator::runInteractive() {
    logger.log(LANG("app.session_started"));
    
    while (true) {
        showMenu();
        string choice;
        getline(cin, choice);
        cout << Colors::CYAN << "╰─────────────────────────────────────────────────────────────╯" << Colors::NC << endl;
        
        if (choice == "1") {
            system("clear");
            sync();
        } else if (choice == "2") {
            system("clear");
            checkPurpur();
        } else if (choice == "3") {
            system("clear");
            sync();
            checkPurpur();
        } else if (choice == "4") {
            system("clear");
            checkPluginUpdates();
        } else if (choice == "5") {
            system("clear");
            downloadPluginUpdates();
        } else if (choice == "6") {
            system("clear");
            backup(prodServerPath, "production_manual");
        } else if (choice == "7") {
            system("clear");
            backup(testServerPath, "test_manual");
        } else if (choice == "8") {
            system("clear");
            migrateToProduction();
        } else if (choice == "9") {
            system("clear");
            logger.viewRecentLogs();
        } else if (choice == "10") {
            system("clear");
            viewRecentBackups();
        } else if (choice == "11") {
            system("clear");
            showPluginUpdates();
        } else if (choice == "12") {
            system("clear");
            showConfiguration();
        } else if (choice == "13") {
            system("clear");
            cout << "Plugin Configuration Management:" << endl;
            cout << "1. Scan and configure from existing plugins" << endl;
            cout << "2. Generate config from plugin-list.txt" << endl;  
            cout << "3. Reload plugin configuration" << endl;
            cout << "4. Back to main menu" << endl;

            string subChoice;
            cout << "Choose option: ";
            getline(cin, subChoice);

            if (subChoice == "1") {
                string pluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
                pluginManager.scanAndConfigurePlugins(pluginsPath);
            } else if (subChoice == "2") {
                // @todo DEPRECATE
                pluginManager.generateConfigFromPluginList("data/plugin-list.txt");
            } else if (subChoice == "3") {
                pluginManager.initFromConfig(Utils::getDataPath("plugins.json"));
            }
        } else if (choice == "0") {
            logger.log(LANG("app.session_ended"));
            logger.log(LANG("app.goodbye"));
            break;
        } else {
            logger.err(LANG("menu.invalid_option"));
        }
        
        cout << endl << Colors::CYAN << LANG("menu.press_enter") << Colors::NC << endl;
        cin.get();
    }
}

void Pluginator::showConfiguration() {
    Config::getInstance().printConfig();
    cout << "\n" << Colors::BLUE << "Environment Variables:" << Colors::NC << endl;
    cout << "Set any of these to override config file values:" << endl;
    cout << "  PLUGINATOR_PROD_PATH" << endl;
    cout << "  PLUGINATOR_TEST_PATH" << endl;
    cout << "  PLUGINATOR_BACKUP_DIR" << endl;
    cout << "  PLUGINATOR_LOGS_DIR" << endl;
    cout << "  PLUGINATOR_MAX_BACKUPS" << endl;
    cout << "  PLUGINATOR_MAX_LOG_DAYS" << endl;
    cout << "  PLUGINATOR_MINECRAFT_VERSION" << endl;
    cout << endl;
    
    // show current language setting
    cout << Colors::BLUE << "Language Settings:" << Colors::NC << endl;
    cout << "  Current language: " << Lang::getInstance().getLanguage() << endl;
    cout << "  Available languages: en, es" << endl;
    cout << endl;
    
    // show plugin summary
    cout << Colors::BLUE << "Plugin Summary:" << Colors::NC << endl;
    cout << "  Total configured plugins: " << plugins.size() << endl;
    cout << "  Plugin versions file: " << pluginVersionsFile << endl;
    cout << endl;
}

/**
 * sync
 * @brief function to sync plugins from production server to test server
 */
void Pluginator::sync() {
    logger.log(LANG("sync.starting"));

    string prodPluginsPath = prodServerPath + "/plugins";
    string testPluginsPath = testServerPath + "/plugins";

    // confirm plugins dir exists for both servers
    if (!exists(prodPluginsPath)) {
        logger.err(LANGF("error.directory_not_found", prodPluginsPath));
        return;
    }

    if (!exists(testPluginsPath)) {
        logger.err(LANGF("error.directory_not_found", testPluginsPath));
        return;
    }

    // create backup of test server plugins
    backup(testPluginsPath, "test_plugins_before_sync");
    
    // remove old jar files from test server
    logger.log(LANG("sync.removing_jars"));
    int removedCount = 0;
    for (const auto& entry : directory_iterator(testPluginsPath)) {
        if (entry.path().extension() == ".jar") {
            try {
                remove(entry.path());
                logger.log(LANGF("sync.removed", entry.path().filename().string()));
                removedCount++;
            } catch (const exception& e) {
                logger.warn(LANGF("error.failed_to_remove", entry.path().filename().string()));
            }
        }
    }

    // copy jar files from production server to test server
    logger.log(LANG("sync.copying_jars"));
    int copiedCount = 0;
    bool foundJars = false;
    for (const auto& entry : directory_iterator(prodPluginsPath)) {
        if (entry.path().extension() == ".jar") {
            foundJars = true;
            try {
                path destination = path(testPluginsPath) / entry.path().filename();
                copy_file(entry.path(), destination);
                logger.log("Copied: " + entry.path().filename().string());
                copiedCount++;
            } catch (const exception& e) {
                logger.warn("Failed to copy: " + entry.path().filename().string());
            }
        }
    }

    logger.success(LANGF("sync.completed", to_string(copiedCount)));

    // disable plugins that can't be running on the test server
    disablePluginsForTest();
}

/**
 * migrateToProduction
 * @brief function to migrate Test Server plugins to Production Server
 */
void Pluginator::migrateToProduction() {
    string prodPluginsPath = prodServerPath + "/plugins";
    string testPluginsPath = testServerPath + "/plugins";

    // confirm plugins dir exists for both servers
    if (!exists(prodPluginsPath)) {
        logger.err(LANGF("error.directory_not_found", prodPluginsPath));
        return;
    }

    if (!exists(testPluginsPath)) {
        logger.err(LANGF("error.directory_not_found", testPluginsPath));
        return;
    }

    // send confirmation msg
    logger.warn(LANG("migrate.warning"));
    string confirm = logger.prompt(LANG("migrate.confirm"));
    
    // if "y" isn't inputted, cancel the function, and exit
    if (confirm != "y" && confirm != "Y") {
        logger.log(LANG("migrate.cancelled"));
        return;
    }

    // create production backup
    logger.log(LANG("migrate.backup"));
    backup(prodServerPath, "prod_plugins_before_migration");

    // start the migration
    logger.log(LANG("migrate.starting"));

    // remove the .DIS extentions from test before copying
    enablePluginsForTest();

    // copy jar files from test to prod
    int copiedCount = 0;
    bool foundJars = false;

    for (const auto& entry : directory_iterator(testPluginsPath)) {
        if (entry.path().extension() == ".jar") {
            foundJars = true;
            try {
                path destination = path(prodPluginsPath) / entry.path().filename();
                copy_file(entry.path(), destination, copy_options::overwrite_existing);
                logger.log("Migrated: " + entry.path().filename().string());
                copiedCount++;
            } catch (const exception& e) {
                logger.warn("Failed to migrate: " + entry.path().filename().string());
            }
        }
    }
    
    if (!foundJars) {
        logger.warn(LANG("migrate.no_jars"));
    }

    // copy server jar if it was updated
    bool serverJarCopied = false;
    for (const auto& entry : directory_iterator(testServerPath)) {
        string filename = entry.path().filename().string();
        if (filename.find("purpur-") == 0 && entry.path().extension() == ".jar") {
            try {
                path destination = path(prodServerPath) / entry.path().filename();
                copy_file(entry.path(), destination, copy_options::overwrite_existing);
                logger.log(LANG("migrate.purpur.copied"));
                serverJarCopied = true;
            } catch (const exception& e) {
                logger.warn(LANGF("migrate.purpur.fail", filename));
            }
        }
    }

    // if all is good, display success, and exit
    logger.success(LANGF("migrate.completed", to_string(copiedCount) + " plugins" + (serverJarCopied ? " and server jar" : "")));
}

/**
 * disablePluginsForTest
 * @brief function to add a "dis" extention for any plugin we don't want running on the Test Server
 */
void Pluginator::disablePluginsForTest() {
    logger.log(LANG("plugin.disabled.starting"));

    string testPluginsPath = testServerPath + "/plugins";
    int disabledCount = 0;

    for (const Plugin& plugin : plugins) {
        if (plugin.disableOnTest) {
            // find the matching files and add .DIS extension
            for (const auto& entry : directory_iterator(testPluginsPath)) {
                if (!entry.is_regular_file()) { continue; }

                // pattern matching
                string filename = entry.path().filename().string();
                string searchPattern = plugin.name.substr(0, min(3, (int)plugin.name.length()));
                if (filename.find(searchPattern) != string::npos && (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".jar")) {
                    try {
                        path newPath = entry.path().string() + ".DIS";
                        rename(entry.path(), newPath);
                        logger.log(LANGF("plugin.disabled.count", filename, newPath.filename().string()));
                        disabledCount++;
                    } catch (const exception& e) {
                        logger.warn(LANGF("plugin.disabled.fail", filename));
                    }
                }
            }
        }
    }

    if (disabledCount > 0) {
        logger.success(LANGF("plugin.disabled.completed", to_string(disabledCount)));
    } else {
        logger.log(LANG("plugin.disabled.no_need"));
    }
}

/**
 * enableAllPluginsInTest
 * @brief function to rename
 */
void Pluginator::enablePluginsForTest() {
    logger.log(LANG("plugin.enabled.starting"));

    string testPluginsPath = testServerPath + "/plugins";

    // confirm plugin path exists for test server
    if (!exists(testPluginsPath)) {
        logger.err(LANGF("error.directory_not_found", testPluginsPath));
        return;
    }

    int enabledCount = 0;
    for (const auto& entry : directory_iterator(testPluginsPath)) {
        string filename = entry.path().filename().string();
        if (endsWith(filename, ".jar.DIS")) {
            try {
                string newName = filename.substr(0, filename.length() - 4);
                path newPath = entry.path().parent_path() / newName;
                rename(entry.path(), newPath);
                logger.log(LANGF("plugin.enabled.count", filename, newName));
                enabledCount++;
            } catch (const exception& e) {
                logger.warn(LANGF("plugin.enabled.fail", filename));
            }
        }
    }

    if (enabledCount > 0) {
        logger.log(LANGF("plugin.enabled.completed", to_string(enabledCount)));
    } else {
        logger.log(LANG("plugin.enabled.no_need"));
    }
    
}