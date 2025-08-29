#pragma once
#include <string>
#include <map>

using namespace std;

class Config {
    static Config* instance;
    map<string, string> settings;

    Config();
    void loadFromEnv();
    void loadFromFile(const string& filename);
    string getEnvVar(const string& name, const string& defaultValue = "");

public:
    static Config& getInstance();

    // getters for settings
    string getProdServerPath() const;
    string getTestServerPath() const;
    string getBackupDir() const;
    string getLogsDir() const;
    string getPluginVersionsFile() const;
    string getMinecraftVersion() const;
    int getMaxBackups() const;
    int getMaxLogDays() const;

    // generic getter and setter
    string get(const string& key, const string& defaultValue = "") const;
    void set(const string& key, const string& value);

    // load/save config
    void loadConfig(const string& filename = "pluginator.config");
    void saveConfig(const string& filename = "pluginator.config");

    // print current config
    void printConfig() const;
};