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
#include <thread>

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
    backupDir = Utils::getDataPath("backups");
    pluginVersionsFile = config.getPluginVersionsFile();
    maxBackups = config.getMaxBackups();

    // create backup dir if it doesn't exist
    if (!backupDir.empty() && !std::filesystem::exists(backupDir)) {
        create_directories(backupDir);
    }

    // load and initialize plugins
    initPlugins();
    pluginManager.initPluginVersions();

    logger.debug(LANG("app.session_started"));
    logger.debug(LANGF("app.prod", prodServerPath));
    logger.debug(LANGF("app.test", testServerPath));
    logger.debug(LANGF("app.backups", backupDir));
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

        logger.debug(LANGF("plugin.init", to_string(plugins.size())));
        return;
    }

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
    logger.log(LANGF("backup.backup_file", backupName));

    // create backup dir if it doesn't exist
    try {
        create_directories(backupDir);
        logger.debug(LANGF("backup.dir_create", backupDir));
    } catch (const exception& e) {
        logger.err(LANGF("backup.dir_create_failed", string(e.what())));
        return;
    }

    // create timestamp and backup file name
    string timestamp = getCurrentTimestamp();
    string backupFile = backupDir + "/" + backupName + "_" + timestamp + ".tar.gz";

    // check if src path exists
    if (!exists(serverPath)) {
        logger.err(LANGF("backup.source_path_dne", serverPath));
        return;
    }

    // debug path information
    logger.debug(LANGF("backup.source_path", serverPath));
    logger.debug(LANGF("backup.parent_path", path(serverPath).parent_path().string()));
    logger.debug(LANGF("backup.filename", path(serverPath).filename().string()));
    logger.debug(LANGF("backup.file", backupFile));

    // get source directory size for progress calculation
    uintmax_t totalSize = 0;
    try {
        for (auto& entry : recursive_directory_iterator(serverPath)) {
            if (entry.is_regular_file()) {
                totalSize += entry.file_size();
            }
        }
    } catch (const exception& e) {
        logger.warn(LANGF("backup.calc_failed", string(e.what())));
        totalSize = 100 * 1024 * 1024; // fallback estimate
    }

    // create tar command
    string command = "tar -czf \"" + backupFile + "\" -C \"" +
                    path(serverPath).parent_path().string() + "\" \"" +
                    path(serverPath).filename().string() + "\"";

    // start tar in background and monitor progress
    logger.log(LANGF("backup.running_command", command));

    atomic<bool> tarFinished{false};
    thread tarThread([command, &tarFinished]() {
        system(command.c_str());
        tarFinished = true;
    });
    
    // progress monitoring
    ProgressBar progressBar(100, "Creating Backup", 40);
    
    // timeout variables
    auto timeoutStart = chrono::steady_clock::now();
    auto timeout = chrono::minutes(5);

    while (!tarFinished) {
        if (exists(backupFile)) {
            uintmax_t currentSize = file_size(backupFile);
            
            // estimate compression - assume 50% compression as reasonable estimate
            uintmax_t estimatedFinalSize = totalSize / 2;
            int percentage = min(95, (int)((currentSize * 100) / estimatedFinalSize));
            
            progressBar.update(percentage, Utils::formatBytes(currentSize));
        }
        
        this_thread::sleep_for(chrono::milliseconds(500));
        
        // check timeout
        if (chrono::steady_clock::now() - timeoutStart > timeout) {
            logger.warn(LANG("backup.taking_longer"));
            break;
        }
    }

    tarThread.join();
    progressBar.finish("Backup complete!");
    
    // check if the file exists
    int result = 0;
    if (!exists(backupFile)) {
        result = 1; // file doesn't exist = fail
    } else {
        uintmax_t finalSize = file_size(backupFile);
        if (finalSize < 1024) { // sus
            result = 1;
        }
    }

    // check perms on backup directory  
    auto perms = status(backupDir).permissions();
    logger.debug(LANGF("backup.dir_permissions", to_string((int)perms)));

    // handle different exit codes
    if (result == 0) {
        logger.success(LANGF("backup.created", backupFile));
        cleanupOldBackups(backupName);
    } else {
        logger.err(LANGF("backup.failed_exit_code", to_string(result)));
        logger.err(LANG("backup.permissions_issue"));
        // try to remove incomplete backup file
        if (exists(backupFile)) {
            remove(backupFile);
            logger.debug(LANG("backup.timeout_cleanup"));
        }
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
    logger.log(LANGF("backup.cleanup", to_string(maxBackups)));

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
            logger.log(LANGF("backup.removed", backupFiles[i].first.filename().string()));
            removedCount++;
        } catch (const exception& e) {
            logger.warn(LANGF("backup.removed_failed", backupFiles[i].first.string()));
        }
    }

    if (removedCount > 0) {
        logger.log(LANGF("backup.cleanup_completed", to_string(removedCount)));
    }
}

/**
 * viewRecentBackups
 * @brief function to view the recently made backups
 */
void Pluginator::viewRecentBackups() {
    cout << "\n" << Colors::BLUE << LANG("backup.recents") << Colors::NC << endl;
    
    if (!exists(backupDir)) {
        logger.log(LANGF("backup.dir_not_found", backupDir));
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
        logger.log(LANG("backup.no_backups"));
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

void Pluginator::viewRecentLogs() {
    logger.viewRecentLogs();
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
            viewRecentLogs();
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
            cout << LANG("menu.interactive.config_title") << endl;
            cout << LANG("menu.interactive.config_1") << endl;
            cout << LANG("menu.interactive.config_2") << endl;
            cout << LANG("menu.interactive.config_3") << endl;

            string subChoice;
            cout << LANG("menu.interactive.choose");
            getline(cin, subChoice);

            if (subChoice == "1") {
                string pluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
                pluginManager.scanAndConfigurePlugins(pluginsPath);
            } else if (subChoice == "2") {
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
    
    // show current language setting
    cout << Colors::BLUE << LANG("config.lang_settings") << Colors::NC << endl;
    cout << LANG("config.lang_current") << Lang::getInstance().getLanguage() << endl;
    cout << LANG("config.lang_available") << endl;
    cout << endl;
    
    // show plugin summary
    cout << Colors::BLUE << LANG("config.plugin_summary") << Colors::NC << endl;
    cout << LANG("config.plugin_total") << plugins.size() << endl;
    cout << LANG("config.plugin_versions_file") << pluginVersionsFile << endl;
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
        logger.err(LANGF("error.dir_not_found", prodPluginsPath));
        return;
    }

    if (!exists(testPluginsPath)) {
        logger.err(LANGF("error.dir_not_found", testPluginsPath));
        return;
    }

    // create backup of test server plugins
    backup(testPluginsPath, "test_plugins_before_sync");
    
    // remove old jar files from test server
    logger.log(LANG("sync.removing_jars"));
    int removedCount = 0;
    for (const auto& entry : directory_iterator(testPluginsPath)) {
        if (entry.path().extension() == ".jar" || entry.path().extension() == ".DIS") {
            try {
                remove(entry.path());
                logger.debug(LANGF("sync.removed", entry.path().filename().string()));
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
                logger.debug(LANGF("sync.copied", entry.path().filename().string()));
                copiedCount++;
            } catch (const exception& e) {
                logger.warn(LANGF("error.failed_to_copy", entry.path().filename().string()));
            }
        }
    }

    if (!foundJars) {
        logger.warn(LANG("sync.no_jars"));
        return;
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
        logger.err(LANGF("error.dir_not_found", prodPluginsPath));
        return;
    }

    if (!exists(testPluginsPath)) {
        logger.err(LANGF("error.dir_not_found", testPluginsPath));
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
    logger.info(LANG("migrate.starting"));

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
                logger.debug(LANGF("migrate.info", entry.path().filename().string()));
                copiedCount++;
            } catch (const exception& e) {
                logger.warn(LANGF("migrate.failed", entry.path().filename().string()));
            }
        }
    }
    
    if (!foundJars) {
        logger.warn(LANG("migrate.no_jars"));
    }

    // disable plugins in test server again
    disablePluginsForTest();

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
 * @brief function to add a ".DIS" extension for any plugin we don't want running on the Test Server
 */
void Pluginator::disablePluginsForTest() {
    logger.info(LANG("plugin.disabled.starting"));

    string testPluginsPath = testServerPath + "/plugins";
    int disabledCount = 0;

    // debug: show what plugins should be disabled
    vector<Plugin> pluginsToDisable;
    for (const Plugin& plugin : plugins) {
        if (plugin.disableOnTest) {
            pluginsToDisable.push_back(plugin);
            logger.debug(LANGF("plugin.disabled.marked",  plugin.name));
        }
    }
    
    logger.debug(LANGF("plugin.disabled.marked.found", to_string(pluginsToDisable.size())));
    
    if (pluginsToDisable.empty()) {
        logger.log(LANG("plugin.disabled.no_need"));
        return;
    }

    // check each plugin that should be disabled
    for (const Plugin& plugin : pluginsToDisable) {
        logger.debug(LANGF("plugin.disabled.processing", plugin.name));
        
        // use existing robust matching logic
        string foundFile = pluginManager.findMatchingPluginFile(plugin.name, testPluginsPath);
        
        if (!foundFile.empty()) {
            logger.debug(LANGF("plugin.version.detect.match", foundFile));
            
            try {
                path currentPath = path(testPluginsPath) / foundFile;
                path newPath = currentPath.string() + ".DIS";
                
                logger.debug(LANGF("plugin.disabled.current_path", currentPath.string()));
                logger.debug(LANGF("plugin.disabled.new_path", newPath.string()));
                
                // check if source file exists
                if (!exists(currentPath)) {
                    logger.warn(LANGF("plugin.disabled.source_dne", currentPath.string()));
                    continue;
                }
                
                // check if .DIS file already exists
                if (exists(newPath)) {
                    logger.debug(LANGF("plugin.disabled.already_dis", foundFile));
                    disabledCount++; // count as disabled
                    continue;
                }
                
                // attempt the rename
                rename(currentPath, newPath);
                
                // verify it worked
                if (exists(newPath) && !exists(currentPath)) {
                    logger.log(LANGF("plugin.disabled.count", foundFile, newPath.filename().string()));
                    disabledCount++;
                } else {
                    logger.err(LANGF("plugin.disabled.rename_failed", foundFile));
                }
                
            } catch (const exception& e) {
                logger.err(LANGF("plugin.disabled.failed", foundFile, string(e.what())));
            }
        } else {
            logger.warn(LANGF("plugin.disabled.no_matching_file", plugin.name));
            logger.debug(LANGF("plugin.disabled.searched_in", testPluginsPath));
            
            // debug: show what files are actually in the directory
            logger.debug(LANG("plugin.available_jars"));
            for (const auto& entry : directory_iterator(testPluginsPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".jar") {
                    logger.debug("  " + entry.path().filename().string());
                }
            }
        }
    }

    if (disabledCount > 0) {
        logger.success(LANGF("plugin.disabled.completed", to_string(disabledCount)));
    } else {
        logger.warn(LANG("plugin.disabled.no_dis"));
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
        logger.err(LANGF("error.dir_not_found", testPluginsPath));
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