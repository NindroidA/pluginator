#pragma once
#include "plugin.hpp"
#include "http_client.hpp"
#include "logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

struct PluginUpdateInfo {
    string version;
    string downloadUrl;
    string filename;
    bool hasUpdate;
    string error;

    PluginUpdateInfo() : hasUpdate(false) {}
};

class PluginManager {
    HttpClient httpClient;
    Logger& logger;
    string pluginVersionsFile;
    map<string, string> storedVersions;
    vector<Plugin> plugins;

    // version file management
    bool loadPluginVersions();
    bool savePluginVersions();
    string extractVersionFromFilename(const string& filename);

    // external config
    bool loadPluginConfigs(const string& configFile = "plugins.json");
    Plugin parsePluginFromJson(const map<string, string>& pluginData);
    bool savePluginConfigs(const string& configFile = "plugins.json");

    // auto detection
    vector<Plugin> detectInstalledPlugins(const string& pluginsPath);
    string guessPluginType(const string& pluginName);

    PluginUpdateInfo checkPluginUpdateSilent(const Plugin& plugin);

    // api integrations
    PluginUpdateInfo checkSpigetAPI(const string& resourceId);
    PluginUpdateInfo checkModrinthAPI(const string& projectSlug, const string& mcVersion);
    PluginUpdateInfo checkGitHubAPI(const string& repo);
    PluginUpdateInfo checkJenkinsAPI(const string& type, const string& job);

    // version comparison
    bool isNewerVersion(const string& current, const string& latest);

    // download
    bool downloadPlugin(const PluginUpdateInfo& updateInfo, const string& outPath);
    void showDownloadProgress(double p);

public:
    PluginManager(Logger& log);

    const vector<Plugin>& getLoadedPlugins() const;

    // config management
    bool initFromConfig(const string& configFile = "plugins.json");
    bool addPlugin(const Plugin& plugin, bool saveToConfig = true);
    bool removePlugin(const string& pluginName, bool saveToConfig = true);
    bool updatePluginConfig(const string& pluginName, const Plugin& newConfig);

    // auto configuration and version management
    void scanAndConfigurePlugins(const string& pluginsPath);
    void scanAndUpdateVersions(const string& pluginsPath);
    void scanAndUpdateVersionsWithAPI(const string& pluginsPath);
    void verifyVersionsWithAPI(const string& pluginsPath);
    string detectPluginVersion(const Plugin& plugin, const string& pluginsPath);
    void generateConfigFromPluginList(const string& pluginListFile);

    // core plugin managements
    void initPluginVersions();
    string getStoredVersion(const string& pluginName);
    void setStoredVersion(const string& pluginName, const string& version);

    // updates
    PluginUpdateInfo checkPluginUpdate(const Plugin& plugin);
    void checkAllPluginUpdates(const vector<Plugin>& plugins, bool downloadUpdates = false);
    
    // plugin status
    void showPluginStatus(const vector<Plugin>& plugins);
    
    // individual plugin operations
    bool updatePlugin(const Plugin& plugin, const string& testServerPath);
    bool downloadPluginToPath(const Plugin& plugin, const string& path);
};