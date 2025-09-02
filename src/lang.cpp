#include "../include/lang.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

Lang* Lang::instance = nullptr;

Lang::Lang() : currentLanguage("en") {
    // get the lang file location based on where the program build is running
    auto binaryPath = std::filesystem::canonical("/proc/self/exe");
    auto installDir = binaryPath.parent_path().parent_path();
    string langFile = installDir / "lang" / "en.json";
    
    loadFromJson(langFile);
}

/**
 * getInstance
 * @brief function to get the current lang instance
 * @returns reference to the lang instance
 */
Lang& Lang::getInstance() {
    if (instance == nullptr) {
        instance = new Lang();
    }
    return *instance;
}

/**
 * loadFromJson
 * @brief function to load lang strings from a json file
 * @param filename string reference to the name of the file
 */
void Lang::loadFromJson(const string& filename) {
    // simple json parser thingy
    ifstream file(filename);
    if (!file.is_open()) {
        // if there is no file, load the defaults
        loadDefaultStrings();
        return;
    }

    string line;
    while (getline(file, line)) {
        // skip comments and empty lines
        if (line.empty() || line.find("//") == 0 || line.find("#") == 0) { continue; }

        // "key": "value"
        size_t colonPos = line.find(":");
        if (colonPos != string::npos) {
            // extract key (remove quotes, whitespace)
            string key = line.substr(0, colonPos);
            key = regex_replace(key, regex("[\"'\\s]"), "");

            // extract value (remove quotes, comma, trim)
            string value = line.substr(colonPos + 1);
            value = regex_replace(value, regex("^\\s*[\"']"), "");
            value = regex_replace(value, regex("[\"'],?\\s*$"), ""); 

            if (!key.empty() && !value.empty()) {
                strings[key] = value;
            }
        }
    }
    file.close();
}

/**
 * loadDefaultStrings
 * @brief redundant function to load default hard coded lang strings (Englsih)
 */
void Lang::loadDefaultStrings() {
    // default english strings
    strings["app.title"] = "🎮 Pluginator C++";
    strings["app.version"] = "Version";
    strings["app.goodbye"] = "Farewell gamer! 🎮";
    strings["app.session_started"] = "Pluginator session started!";
    strings["app.session_ended"] = "Pluginator session ended!";
    
    // menu strings
    strings["menu.update_operations"] = "🔄 UPDATE OPERATIONS:";
    strings["menu.backup_operations"] = "💾 BACKUP OPERATIONS:";
    strings["menu.deployment"] = "🔄 DEPLOYMENT:";
    strings["menu.server_management"] = "⚙️  SERVER MANAGEMENT:";
    strings["menu.exit"] = "🚪 Exit";
    strings["menu.choose_option"] = "Choose an option: ";
    strings["menu.press_enter"] = "Press Enter to continue ...";
    strings["menu.invalid_option"] = "Invalid option. Please choose 0-12.";
    
    // operations
    strings["sync.starting"] = "Syncing plugins from Production → Test ...";
    strings["sync.completed"] = "Plugin sync completed! Copied {0} plugins";
    strings["sync.removing_jars"] = "Removing old JAR files from test server ...";
    strings["sync.copying_jars"] = "Copying plugins from Production → Test ...";
    strings["sync.no_jars"] = "No jar files found in production server directory!";
    strings["sync.removed"] = "Removed: {0}";
    strings["sync.copied"] = "Copied: {0}";
    
    // backup strings
    strings["backup.creating"] = "Creating backup: {0}";
    strings["backup.created"] = "Backup created: {0}";
    strings["backup.failed"] = "Failed to create backup!";
    strings["backup.cleanup"] = "Cleaning up old backups (keeping {0} newest) ...";
    strings["backup.cleanup_completed"] = "Cleaned up {0} old backup files";
    strings["backup.no_backups"] = "No backups found.";
    strings["backup.recent_title"] = "📋 Recent Backups:";
    strings["backup.directory_not_found"] = "Backup directory not found: {0}";
    
    // migration strings
    strings["migrate.starting"] = "🚚 Migrating plugins from Test → Production ...";
    strings["migrate.warning"] = "This will replace production with Test Server state!";
    strings["migrate.confirm"] = "Are you sure? (y/n): ";
    strings["migrate.cancelled"] = "Migration cancelled.";
    strings["migrate.backup"] = "Creating production backup before migration ...";
    strings["migrate.plugins_copied"] = "Copied {0} plugins";
    strings["migrate.no_jars"] = "No jar files found in test server plugins directory";
    strings["migrate.purpur.copied"] = "Purpur jar copied to production.";
    strings["migrate.purpur.fail"] = "Failed to copy server jar: {0}";
    strings["migrate.completed"] = "Migration completed! Copied {0}";
    
    // plugin strings
    strings["plugin.disabled.starting"] = "🚫 Disabling selected plugins for Test Server ...";
    strings["plugin.disabled.count"] = "Disabled: {0} → {1}";
    strings["plugin.disabled.fail"] = "Failed to disable: {0}";
    strings["plugin.disabled.no_need"] = "No plugins were re-enabled.";
    strings["plugin.disabled.completed"] = "Re-enabled {0} plugins";
    strings["plugin.enabled.starting"] = "";
    strings["plugin.enabled.count"] = "Re-enabled: {0} → {1}";
    strings["plugin.checking"] = "Checking {0} ...";
    strings["plugin.up_to_date"] = "Up to date";
    strings["plugin.update_available"] = "Update available!";
    strings["plugin.new_plugin"] = "New plugin (not tracked yet)";
    strings["plugin.checking_all"] = "Checking all plugin updates...";
    strings["plugin.versions_file_created"] = "Plugin versions file created";
    strings["plugin.versions_loaded"] = "Loaded {0} plugin versions";
    strings["plugin.version_updated"] = "Updated {0} version to {1}";
    strings["plugin.configurations_loaded"] = "Loaded {0} plugin configurations";
    strings["plugin.downloading"] = "Downloading {0} ...";
    strings["plugin.download_success"] = "Successfully downloaded {0}";
    strings["plugin.download_failed"] = "Failed to download {0}";
    strings["plugin.check_failed"] = "Failed to check {0}: {1}";
    strings["plugin.already_updated"] = "{0} is already up to date";
    strings["plugin.old_removed"] = "Removed old version: {0}";
    strings["plugin.remove_failed"] = "Failed to remove old version: {0}";
    strings["plugin.updated_successfully"] = "Updated successfully";
    strings["plugin.update_failed"] = "Update failed";
    strings["plugin.update_summary"] = "{0} updates available, {1} errors";
    strings["plugin.update_summary_downloaded"] = "Downloaded {0} plugin updates";

    // purpur settings
    strings["purpur.checking"] = "Checking for Purpur updates ...";
    strings["purpur.latest_version"] = "Latest version: {0}";
    strings["purpur.latest_build"] = "Latest build: {0}";
    strings["purpur.build_result"] = "Build result: {0}";
    strings["purpur.build_failed"] = "Latest build failed. Check back later for a successful build.";
    strings["purpur.downloading"] = "Downloading Purpur version {0} ...";
    strings["purpur.download_success"] = "Successfully downloaded Purpur {0}";
    strings["purpur.download_failed"] = "Failed to download Purpur {0}";
    strings["purpur.download_prompt"] = "Would you like to download the latest version? (y/n): ";
    strings["purpur.downloaded_cancelled"] = "Download cancelled.";
    strings["purpur.downloaded_to"] = "Purpur downloaded to: {0}";
    
    // error strings
    strings["error.directory_not_found"] = "Directory not found: {0}";
    strings["error.file_not_found"] = "File not found: {0}";
    strings["error.failed_to_copy"] = "Failed to copy: {0}";
    strings["error.failed_to_remove"] = "Failed to remove: {0}";
    
    // log strings
    strings["log.recent_title"] = "📋 Recent Log Files:";
    strings["log.latest_entries"] = "Latest log entries:";
    strings["log.no_entries"] = "No entries in today's log yet.";
    strings["log.cleanup"] = "Cleaned up {0} old log files (older than {1} days)";
}

/**
 * get
 * @brief generic getter function to get the value of a key from a lang json file
 * @param key string reference to the name of the lang key
 * @param defaultValue string reference to the default value of the given key
 */
string Lang::get(const string& key, const string& defaultValue) const {
    auto it = strings.find(key);
    if (it != strings.end()) { return it -> second; }
    return defaultValue.empty() ? key : defaultValue;
}

/**
 * format
 * @brief generic format function to format parameters for a lang string
 * @param key string reference to the name of the lang key
 * @param arg1 string reference to the first required argument
 */
string Lang::format(const string& key, const string& arg1) const {
    string str = get(key);
    size_t pos = str.find("{0}");
    if (pos != string::npos) {
        str.replace(pos, 3, arg1);
    }
    return str;
}

/**
 * format
 * @brief generic format function to format parameters for a lang string
 * @param key string reference to the name of the lang key
 * @param arg1 string reference to the first required argument
 * @param arg2 string reference to the second required argument
 */
string Lang::format(const string& key, const string& arg1, const string& arg2) const {
    string str = format(key, arg1);
    size_t pos = str.find("{1}");
    if (pos != string::npos) {
        str.replace(pos, 3, arg2);
    }
    return str;
}

/**
 * format
 * @brief generic format function to format parameters for a lang string
 * @param key string reference to the name of the lang key
 * @param arg1 string reference to the first required argument
 * @param arg2 string reference to the second required argument
 * @param arg3 string reference to the third required argument
 */
string Lang::format(const string& key, const string& arg1, const string& arg2, const string& arg3) const {
    string str = format(key, arg1, arg2);
    size_t pos = str.find("{2}");
    if (pos != string::npos) {
        str.replace(pos, 3, arg3);
    }
    return str;
}

string Lang::operator[](const string& key) const {
    return get(key);
}

/**
 * setLanguage
 * @brief function to set the language of the program
 * @param lang string reference to the desired lang (short form)
 */
void Lang::setLanguage(const string& lang) {
    currentLanguage = lang;
    string filename = "lang/" + lang + ".json";
    loadFromJson(filename);
}

/**
 * reloadStrings
 * @brief function to reload lang json file
 */
void Lang::reloadStrings() {
    strings.clear();
    loadFromJson("lang/" + currentLanguage + ".json");
}

/**
 * printLoadedStrings
 * @brief function to print all the loaded strings in the program
 */
void Lang::printLoadedStrings() const {
    cout << "\n📋 Loaded Language Strings (" << currentLanguage << "):\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& pair : strings) {
        cout << "  " << pair.first << ": " << pair.second << "\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
}