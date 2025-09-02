#include "../include/config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

Config* Config::instance = nullptr;

Config::Config() {
    // set defaults
    /*
    settings["PROD_SERVER_PATH"] = "./SERVER/prod";
    settings["TEST_SERVER_PATH"] = "./SERVER/test";
    settings["BACKUP_DIR"] = "./data/backups";
    settings["LOGS_DIR"] = "./data/logs";
    settings["PLUGIN_VERSIONS_FILE"] = "./data/plugin_versions.json";
    settings["MAX_BACKUPS"] = "2";
    settings["MAX_LOG_DAYS"] = "30";
    settings["MINECRAFT_VERSION"] = "1.21.8";
    */

    // load from env variables
    //loadFromEnv();

    // try to load config
    loadFromFile("./config/pluginator.config");
}

/**
 * getInstance
 * @brief function to get the current config instance
 * @returns reference to the current config
 */
Config& Config::getInstance() {
    if (instance == nullptr) {
        instance = new Config();
    }
    return *instance;
}

/**
 * @deprecated
 * getEnvVar
 * @brief function to get the value on an environment variable
 * @param name string reference to the name of the environment variable
 * @param defaultValue string reference to the default value of the given key
 * @returns the value as a string
 */
string Config::getEnvVar(const string& name, const string& defaultValue) {
    const char* value = getenv(name.c_str());
    return value ? string(value) : defaultValue;
}

/**
 * @deprecated
 * loadFromEnv
 * @brief function to load environment variables from the env file
 */
void Config::loadFromEnv() {
    // check for env variables with prefix "PLUGINATOR_"
    settings["PROD_SERVER_PATH"] = getEnvVar("PLUGINATOR_PROD_PATH", settings["PROD_SERVER_PATH"]);
    settings["TEST_SERVER_PATH"] = getEnvVar("PLUGINATOR_TEST_PATH", settings["TEST_SERVER_PATH"]);
    settings["BACKUP_DIR"] = getEnvVar("PLUGINATOR_BACKUP_DIR", settings["BACKUP_DIR"]);
    settings["LOGS_DIR"] = getEnvVar("PLUGINATOR_LOGS_DIR", settings["LOGS_DIR"]);
    settings["PLUGIN_VERSIONS_FILE"] = getEnvVar("PLUGINATOR_PLUGIN_VERSIONS", settings["PLUGIN_VERSIONS_FILE"]);
    settings["MAX_BACKUPS"] = getEnvVar("PLUGINATOR_MAX_BACKUPS", settings["MAX_BACKUPS"]);
    settings["MAX_LOG_DAYS"] = getEnvVar("PLUGINATOR_MAX_LOG_DAYS", settings["MAX_LOG_DAYS"]);
    settings["MINECRAFT_VERSION"] = getEnvVar("PLUGINATOR_MINECRAFT_VERSION", settings["MINECRAFT_VERSION"]);
}

/**
 * loadFromFile
 * @brief function to load a config from the specified file
 * @param filename string reference to the name of the file
 */
void Config::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        // skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        // parse key=value
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            settings[key] = value;
        }
    }
    file.close();
}

/* specific config getters */
string Config::getProdServerPath() const { return get("PROD_SERVER_PATH"); }
string Config::getTestServerPath() const { return get("TEST_SERVER_PATH"); }
string Config::getBackupDir() const { return get("BACKUP_DIR"); }
string Config::getLogsDir() const { return get("LOGS_DIR"); }
string Config::getPluginVersionsFile() const { return get("PLUGIN_VERSIONS_FILE"); }
string Config::getMinecraftVersion() const { return get("MINECRAFT_VERSION"); }
int Config::getMaxBackups() const { return stoi(get("MAX_BACKUPS", "2")); }
int Config::getMaxLogDays() const { return stoi(get("MAX_LOG_DAYS", "30")); }

/**
 * get
 * @brief generic getter function to get the value of a config variable
 * @param key string reference to the name of the config variable
 * @param defaultValue string reference to the default value of the given key
 */
string Config::get(const string& key, const string& defaultValue) const {
    auto it = settings.find(key);
    return it != settings.end() ? it -> second : defaultValue;
}

/**
 * set
 * @brief generic setter function to set the value of a config variable
 * @param key string reference to the name of the config variable
 * @param value string reference to the value we want to set to the given key
 */
void Config::set(const string& key, const string& value) {
    settings[key] = value;
}

/**
 * loadConfig
 * @brief function to load the config from a given file
 * @param filename string reference to the name of the file
 */
void Config::loadConfig(const string& filename) {
    // load config from the specified file
    loadFromFile(filename);
    
    // also reload environment variables to ensure they take precedence
    //loadFromEnv();
}

/**
 * saveConfig
 * @brief function to save the current config settings to a local config file
 * @param filename string reference to the name of the file we want to save to
 */
void Config::saveConfig(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;

    file << "# Pluginator Configuration File\n";
    file << "# Environment variables take precedence over these values\n\n";

    for (const auto& pair : settings) {
        file << pair.first << "=" << pair.second << "\n";
    }
    file.close();
}

/**
 * printConfig
 * @brief function to print the current config settings
 */
void Config::printConfig() const {
    cout << "\n📋 Current Configuration:\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& pair : settings) {
        printf("  %-25s: %s\n", pair.first.c_str(), pair.second.c_str());
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
}