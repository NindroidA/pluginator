#include "../include/plugin_manager.hpp"
#include "../include/json_helper.hpp"
#include "../include/config.hpp"
#include "../include/lang.hpp"
#include "../include/colors.hpp"
#include "../include/utils.hpp"
#include "../include/progress_bar.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>
#include <iomanip>

using namespace filesystem;
using namespace std;

PluginManager::PluginManager(Logger& log) : logger(log) {
    pluginVersionsFile = Config::getInstance().getPluginVersionsFile();
    httpClient.setUserAgent("Pluginator (Minecraft Plugin Manager)");
    httpClient.setTimeout(30);
}

/**
 * loadPluginVersions
 * @brief function to load the saved plugin versions
 * @returns boolean value of whether the load was a success or not
 */
bool PluginManager::loadPluginVersions() {
    ifstream file(pluginVersionsFile);
    if (!file.is_open()) {
        return false;
    }
    
    // read entire file
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty() || !JsonHelper::isValidJson(content)) {
        return false;
    }
    
    // parse json
    storedVersions = JsonHelper::parseSimpleObject(content);
    logger.debug(LANGF("plugin.versions_loaded", to_string(storedVersions.size())));
    
    return true;
}

/**
 * getLoadedPlugins
 * @brief Just a simple getter function to get the loaded plugins
 */
const vector<Plugin>& PluginManager::getLoadedPlugins() const { return plugins; }

/**
 * savePluginVersions
 * @brief function to save the plugin versions file
 * @returns boolean value of whether the save was a success or not
 */
bool PluginManager::savePluginVersions() {
    ofstream file(pluginVersionsFile);
    if (!file.is_open()) {
        logger.err(LANGF("error.failed_to_write", pluginVersionsFile));
        return false;
    }
    
    string json = JsonHelper::createObjectPretty(storedVersions);
    file << json << endl;
    file.close();
    
    return true;
}

string PluginManager::extractVersionFromFilename(const string& filename) {
    logger.debug("Extracting version from: " + filename);
    
    // remove .jar extension
    string name = filename;
    if (endsWith(name, ".jar")) {
        name = name.substr(0, name.length() - 4);
    }
    
    logger.debug("  Filename without .jar: " + name);
    
    // updated version patterns to handle your specific files
    vector<pair<regex, string>> patterns = {
        {regex(R"(-([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)$)"), "Standard 4-part version"}, // -1.2.3.4
        {regex(R"(-([0-9]+\.[0-9]+\.[0-9]+)(?:-[A-Za-z].*)?$)"), "Standard 3-part version"}, // -1.2.3
        {regex(R"(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)$)"), "End 4-part version"}, // Jobs5.2.6.0
        {regex(R"(([0-9]+\.[0-9]+\.[0-9]+)$)"), "End 3-part version"}, // CMILib1.5.6.3
        {regex(R"(-([0-9]+\.[0-9]+)(?:-[A-Za-z].*)?$)"), "Standard 2-part version"}, // -1.2
        {regex(R"([vV]([0-9]+\.[0-9]+(?:\.[0-9]+)?)$)"), "Version with v prefix"}, // v1.2.3
    };
    
    for (const auto& [pattern, description] : patterns) {
        smatch match;
        if (regex_search(name, match, pattern)) {
            string version = match[1].str();
            logger.debug("  Matched pattern (" + description + "): " + version);
            return version;
        }
    }
    
    logger.debug("  No version pattern matched");
    return "";
}

/**
 * loadPluginConfigs
 * @brief function to load the saved plugins from the given config file
 * @returns boolean value of whether the load was a success or not
 */
bool PluginManager::loadPluginConfigs(const string& configFile) {
    ifstream file(configFile);
    if (!file.is_open()) {
        logger.warn(LANGF("plugin.config_not_found", configFile));
        return false;
    }

    // read whole file
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    if (!JsonHelper::isValidJson(content)) {
        logger.err(LANGF("plugin.config_invalid_json", configFile));
        return false;
    }

    // parse plugins array - look for "plugins" key
    regex pluginsArrayRegex("\"plugins\"\\s*:\\s*\\[([^\\]]+)\\]");
    smatch arrayMatch;
    
    if (!regex_search(content, arrayMatch, pluginsArrayRegex)) {
        logger.err(LANGF("plugin.config_no_plugins_array", configFile));
        return false;
    }
    
    string pluginsArrayContent = arrayMatch[1].str();
    
    // parse individual plugin objects
    regex pluginObjectRegex("\\{([^\\}]+)\\}");
    sregex_iterator iter(pluginsArrayContent.begin(), pluginsArrayContent.end(), pluginObjectRegex);
    sregex_iterator end;

    plugins.clear();
    int loadedCount = 0;

    for (; iter != end; ++iter) {
        string pluginJson = "{" + iter->str(1) + "}";
        map<string, string> pluginData = JsonHelper::parseSimpleObject(pluginJson);
        
        if (pluginData["enabled"] == "false") {
            logger.debug("Skipping disabled plugin: " + pluginData["name"]);
            continue;
        }
        
        Plugin plugin = parsePluginFromJson(pluginData);
        if (!plugin.name.empty()) {
            plugins.push_back(plugin);
            loadedCount++;
            logger.debug("Loaded plugin: " + plugin.name + " (type: " + plugin.type + ")");
        }
    }

    logger.success(LANGF("plugin.config_loaded", to_string(loadedCount), configFile));
    return true;
}

/**
 * parsePluginFromJson
 * @brief function to parse plugin info from the plugin json fiile
 * @param data map reference of two strings (containing the key and value)
 * @returns plugin object
 */
Plugin PluginManager::parsePluginFromJson(const map<string, string>& data) {
    auto getValue = [&](const string& key, const string& defaultValue = "") {
        auto it = data.find(key);
        return it != data.end() ? it->second : defaultValue;
    };
    
    string name = getValue("name");
    string type = getValue("type");
    
    if (name.empty() || type.empty()) {
        logger.warn(LANGF("plugin.config_missing_required", name));
        return Plugin("", "");
    }
    
    Plugin plugin(name, type);
    plugin.resourceId = getValue("resourceId");
    plugin.projectSlug = getValue("projectSlug"); 
    plugin.filenamePattern = getValue("filenamePattern");
    plugin.mcVersion = getValue("mcVersion", 
                               Config::getInstance().getMinecraftVersion());
    plugin.disableOnTest = (getValue("disableOnTest") == "true");
    
    if (type == "github") {
        plugin.resourceId = getValue("repo");
    }
    
    if (type == "jenkins") {
        plugin.jType = getValue("jType", "froobworld");
        plugin.job = getValue("job");
    }
    
    return plugin;
}

/**
 * initFromConfig
 * @brief function to load/initialize plugins from the given config file
 * @param configFile string reference to the name of the config file
 * @returns boolean value of whether the initialization was successful or not
 */
bool PluginManager::initFromConfig(const string& configFile) {
    logger.debug("Initializing plugin manager from config: " + configFile);

    // load all plugin versions
    if (storedVersions.empty()) {
        initPluginVersions();
    }
    
    // load plugin configurations from json file
    bool configLoaded = loadPluginConfigs(configFile);
    
    // if we couldn't load the config, warn it, and return false
    if (!configLoaded) {
        logger.warn(LANGF("plugin.config_load_failed", configFile));
        return false;
    }
    
    // if all is good, log the success, and return true
    logger.success(LANGF("plugin.manager_initialized", to_string(plugins.size())));
    return true;
}

/**
 * addPlugin
 * @brief function to add a plugin to the plugin config file
 * @param plugin plugin reference to the selected plugin's information
 * @param saveToConfig boolean denoting whether the program was able to save the plugin config or not
 * @returns boolean value of whether the add was successful or not
 */
bool PluginManager::addPlugin(const Plugin& plugin, bool saveToConfig) {
    // Check if plugin already exists
    for (const auto& existingPlugin : plugins) {
        if (existingPlugin.name == plugin.name) {
            logger.warn(LANGF("plugin.already_exists", plugin.name));
            return false;
        }
    }
    
    plugins.push_back(plugin);
    logger.log(LANGF("plugin.added", plugin.name));
    
    if (saveToConfig) {
        return savePluginConfigs();
    }
    
    return true;
}

/**
 * removePlugin
 * @brief function to remove a plugin from the plugin config file
 * @param plugin string reference to the name of the plugin
 * @param saveToConfig boolean denoting whether the program was able to save the config or not
 * @returns boolean value of whether the removal was successful or not
 */
bool PluginManager::removePlugin(const string& pluginName, bool saveToConfig) {
    auto it = find_if(plugins.begin(), plugins.end(), 
                      [&pluginName](const Plugin& p) { return p.name == pluginName; });
    
    if (it == plugins.end()) {
        logger.warn(LANGF("plugin.not_found", pluginName));
        return false;
    }
    
    plugins.erase(it);
    logger.log(LANGF("plugin.removed", pluginName));
    
    if (saveToConfig) {
        return savePluginConfigs();
    }
    
    return true;
}

/**
 * updatePluginConfig
 * @brief function to update the saved info of a given plugin
 * @param pluginName string reference to the name of the plugin
 * @param newConfig plugin reference to the updated plugin information
 * @returns boolean value of whether the update was successful or not
 */
bool PluginManager::updatePluginConfig(const string& pluginName, const Plugin& newConfig) {
    auto it = find_if(plugins.begin(), plugins.end(), 
                      [&pluginName](const Plugin& p) { return p.name == pluginName; });
    
    if (it == plugins.end()) {
        logger.warn(LANGF("plugin.not_found", pluginName));
        return false;
    }
    
    *it = newConfig;
    logger.log(LANGF("plugin.config_updated", pluginName));
    
    return savePluginConfigs();
}

/**
 * scanAndConfigurePlugins
 * @brief function to scan all the jar files in the specified plugins directory
 * @param pluginsPath string reference to the path where the plugins are stored
 */
void PluginManager::scanAndConfigurePlugins(const string& pluginsPath) {
    logger.log(LANGF("plugin.scanning", pluginsPath));
    
    if (!exists(pluginsPath)) {
        logger.err(LANGF("error.directory_not_found", pluginsPath));
        return;
    }
    
    vector<Plugin> detectedPlugins;
    
    for (const auto& entry : directory_iterator(pluginsPath)) {
        if (!entry.is_regular_file()) continue;
        
        string filename = entry.path().filename().string();
        if (!endsWith(filename, ".jar") || endsWith(filename, ".jar.DIS")) continue;
        
        // extract plugin name from filename
        string pluginName = filename;
        
        // remove version numbers and .jar extension
        regex versionPattern(R"(-[0-9\\.]+\\.jar$)");
        pluginName = regex_replace(pluginName, versionPattern, "");
        
        // clean up common patterns
        pluginName = regex_replace(pluginName, regex("[-_]"), " ");
        
        Plugin plugin(pluginName, guessPluginType(pluginName));
        plugin.filenamePattern = pluginName + "*.jar";
        
        detectedPlugins.push_back(plugin);
        logger.log(LANGF("plugin.detected", pluginName, plugin.type));
    }
    
    logger.success(LANGF("plugin.scan_completed", to_string(detectedPlugins.size())));
    
    // offer to save configuration
    string response = logger.prompt(LANG("plugin.save_config_prompt"));
    if (response == "y" || response == "Y") {
        plugins = detectedPlugins;
        savePluginConfigs();
    }
}

void PluginManager::scanAndUpdateVersions(const string& pluginsPath) {
    logger.log("Scanning plugin files to update version tracking...");
    
    if (!exists(pluginsPath)) {
        logger.err("Plugins directory not found: " + pluginsPath);
        return;
    }
    
    int totalPlugins = plugins.size();
    int updatedCount = 0;
    
    ProgressBar progress(totalPlugins, "Scanning files");
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string detectedVersion = detectPluginVersion(plugin, pluginsPath);
        
        if (!detectedVersion.empty() && detectedVersion != "unknown") {
            string currentStored = getStoredVersion(plugin.name);
            
            if (currentStored != detectedVersion) {
                setStoredVersion(plugin.name, detectedVersion);
                logger.debug("Updated " + plugin.name + ": " + currentStored + " -> " + detectedVersion);
                updatedCount++;
            }
        } else {
            logger.debug("Could not detect version for: " + plugin.name);
        }
    }
    
    progress.finish("Updated " + to_string(updatedCount) + " plugin versions.");
}

void PluginManager::scanAndUpdateVersionsWithAPI(const string& pluginsPath) {
    logger.log("Scanning plugin files with API verification...");
    
    if (!exists(pluginsPath)) {
        logger.err("Plugins directory not found: " + pluginsPath);
        return;
    }
    
    int totalPlugins = plugins.size();
    int updatedCount = 0;
    int apiUpdatedCount = 0;
    
    ProgressBar progress(totalPlugins, "Scanning plugins");
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string detectedVersion = detectPluginVersion(plugin, pluginsPath);
        string currentStored = getStoredVersion(plugin.name);
        
        if (!detectedVersion.empty() && detectedVersion != "unknown") {
            if (currentStored != detectedVersion) {
                setStoredVersion(plugin.name, detectedVersion);
                logger.debug("Updated " + plugin.name + " from filename: " + detectedVersion);
                updatedCount++;
            }
        } else {
            if (plugin.type != "manual") {
                logger.debug("No filename version for " + plugin.name + ", checking API...");
                
                PluginUpdateInfo apiInfo = checkPluginUpdate(plugin);
                
                if (!apiInfo.error.empty()) {
                    logger.debug("API check failed for " + plugin.name + ": " + apiInfo.error);
                    continue;
                }
                
                if (!apiInfo.version.empty() && apiInfo.version != "unknown" && 
                    apiInfo.version != "spiget-latest" && apiInfo.version != "latest") {
                    
                    if (currentStored != apiInfo.version) {
                        setStoredVersion(plugin.name, apiInfo.version);
                        logger.debug("Updated " + plugin.name + " from API: " + apiInfo.version);
                        apiUpdatedCount++;
                    }
                }
            }
        }
    }
    
    progress.finish("Scan complete. Updated " + to_string(updatedCount) + 
                   " from filenames, " + to_string(apiUpdatedCount) + " from APIs.");
}

void PluginManager::verifyVersionsWithAPI(const string& pluginsPath) {
    logger.log("Verifying stored versions against installed files...");
    
    int totalPlugins = plugins.size();
    int verifiedCount = 0;
    int correctedCount = 0;
    int newerAvailableCount = 0;
    
    ProgressBar progress(totalPlugins, "Verifying plugins");
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string actualInstalledVersion = detectPluginVersion(plugin, pluginsPath);
        string storedVersion = getStoredVersion(plugin.name);
        
        if (actualInstalledVersion.empty() || actualInstalledVersion == "unknown") {
            logger.debug("Could not detect installed version for: " + plugin.name);
            continue;
        }
        
        if (storedVersion == actualInstalledVersion) {
            verifiedCount++;
            logger.debug("Verified " + plugin.name + " version: " + storedVersion);
        } else {
            setStoredVersion(plugin.name, actualInstalledVersion);
            logger.debug("Corrected " + plugin.name + ": stored=" + storedVersion + 
                        " → actual=" + actualInstalledVersion);
            correctedCount++;
        }
        
        // check for updates
        if (plugin.type != "manual") {
            PluginUpdateInfo apiInfo = checkPluginUpdate(plugin);
            
            if (!apiInfo.error.empty()) {
                logger.debug("API check failed for " + plugin.name + ": " + apiInfo.error);
                continue;
            }
            
            if (!apiInfo.version.empty() && apiInfo.version != "unknown" && 
                apiInfo.version != "spiget-latest" && apiInfo.version != "latest") {
                
                if (isNewerVersion(actualInstalledVersion, apiInfo.version)) {
                    logger.debug("Update available for " + plugin.name + ": " + 
                               actualInstalledVersion + " → " + apiInfo.version);
                    newerAvailableCount++;
                }
            }
        }
    }
    
    string summary = "Verified: " + to_string(verifiedCount) + 
                    ", Corrected: " + to_string(correctedCount) + 
                    ", Updates available: " + to_string(newerAvailableCount);
    
    progress.finish(summary);
    
    logger.success("Verification complete:");
    logger.success("  • " + to_string(verifiedCount) + " versions verified correct");
    logger.success("  • " + to_string(correctedCount) + " stored versions corrected");
    logger.success("  • " + to_string(newerAvailableCount) + " plugins have updates available");
}

string PluginManager::detectPluginVersion(const Plugin& plugin, const string& pluginsPath) {
    logger.debug("Detecting version for plugin: " + plugin.name);
    
    // find the plugin file
    string foundFile;
    
    for (const auto& entry : directory_iterator(pluginsPath)) {
        string filename = entry.path().filename().string();
        
        if (!endsWith(filename, ".jar") || endsWith(filename, ".jar.DIS")) continue;
        
        logger.debug("  Checking file: " + filename);
        
        string pluginNameLower = plugin.name;
        transform(pluginNameLower.begin(), pluginNameLower.end(), pluginNameLower.begin(), ::tolower);
        
        string filenameLower = filename;
        transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);
        
        // try multiple matching strategies
        bool matched = false;
        
        // strat 1: exact filename match
        if (filenameLower == pluginNameLower + ".jar") {
            matched = true;
            logger.debug("    Matched using exact filename");
        }

        // strat 2: plugin name + hyphen
        else if (!matched && filenameLower.find(pluginNameLower + "-") == 0) {
            matched = true;
            logger.debug("    Matched using hyphen start");
        }

        // strat 3: handle suffix removals
        else if (!matched) {
            string baseName = pluginNameLower;
            
            // remove common suffixes and try matching
            vector<string> suffixes = {"-reloaded", "v3", "v2", "-spigot", "-bukkit", "-paper"};
            
            for (const string& suffix : suffixes) {
                if (baseName.length() > suffix.length() && 
                    baseName.substr(baseName.length() - suffix.length()) == suffix) {
                    
                    string nameWithoutSuffix = baseName.substr(0, baseName.length() - suffix.length());
                    
                    if (filenameLower == nameWithoutSuffix + ".jar" ||
                        filenameLower.find(nameWithoutSuffix + "-") == 0) {
                        matched = true;
                        logger.debug("    Matched after removing suffix: " + suffix);
                        break;
                    }
                }
            }
        }

        // strat 4: prevent false positives
        if (matched) {
            // ensure we don't match substrings of longer plugin names
            string fileBase = filename.substr(0, filename.find_last_of('.'));
            transform(fileBase.begin(), fileBase.end(), fileBase.begin(), ::tolower);
            
            // if plugin name is "vault" but file is "forgetfultrialvault", reject it
            if (pluginNameLower.length() < fileBase.length()) {
                string prefix = fileBase.substr(0, pluginNameLower.length());
                if (prefix == pluginNameLower) {
                    char nextChar = fileBase[pluginNameLower.length()];
                    if (nextChar != '-' && nextChar != '.') {
                        matched = false;
                        logger.debug("    Rejected false positive match");
                    }
                }
            }
        }
        
        if (matched) {
            foundFile = filename;
            logger.debug("  Found matching file: " + foundFile);
            break;
        }
    }
    
    if (foundFile.empty()) {
        logger.debug("  No matching file found for: " + plugin.name);
        return "unknown";
    }
    
    // extract version from filename
    string version = extractVersionFromFilename(foundFile);
    logger.debug("  Extracted version: " + version);
    
    return version;
}

/**
 * generateConfigFromPluginList
 * @brief function to generate a plugin config from the scanned plugins
 * @param pluginListFile string reference to the plugin list file
 */
void PluginManager::generateConfigFromPluginList(const string& pluginListFile) {
    ifstream file(pluginListFile);
    if (!file.is_open()) {
        logger.err(LANGF("error.file_not_found", pluginListFile));
        return;
    }
    
    plugins.clear();
    string line;
    int lineNum = 0;
    
    while (getline(file, line)) {
        lineNum++;
        
        // skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // remove any leading/trailing whitespace and special characters
        line = regex_replace(line, regex("^[*\\s]+"), "");
        line = regex_replace(line, regex("[,\\s]+$"), "");
        
        if (line.empty()) continue;
        
        Plugin plugin(line, guessPluginType(line));
        plugin.filenamePattern = line + "*.jar";
        
        // check if it should be disabled on test
        if (line.find("Discord") != string::npos || 
            line.find("CMI") != string::npos ||
            line.find("LiteBans") != string::npos) {
            plugin.disableOnTest = true;
        }
        
        plugins.push_back(plugin);
        logger.log(LANGF("plugin.added_from_list", line, plugin.type));
    }
    
    file.close();
    
    logger.success(LANGF("plugin.list_processed", to_string(plugins.size()), pluginListFile));
    
    // save to json configuration
    if (savePluginConfigs()) {
        logger.success(LANG("plugin.config_generated"));
    }
}

/**
 * initPluginVersions
 * @brief function to initialize saved plugin versions
 */
void PluginManager::initPluginVersions() {
    if (!loadPluginVersions()) {
        logger.log(LANG("plugin.versions_file_created"));
        savePluginVersions(); // create empty file
    }
}

string PluginManager::guessPluginType(const string& pluginName) {
    // plugin mappings
    map<string, string> knownPlugins = {};
    /*
    map<string, string> knownPlugins = {
        {"EssentialsX", "spiget"},
        {"WorldEdit", "github"}, 
        {"ViaVersion", "spiget"},
        {"PlaceholderAPI", "spiget"},
        {"Vault", "spiget"},
        {"ProtocolLib", "spiget"},
        {"Citizens", "spiget"},
        {"Axiom", "modrinth"},
        {"CMI", "manual"},
        {"DiscordSRV", "manual"},
        {"LiteBans", "manual"}
    };
    */    

    // check for exact or partial matches
    for (const auto& known : knownPlugins) {
        if (pluginName.find(known.first) != string::npos) {
            return known.second;
        }
    }
    
    return "manual"; // default to manual if unknown
}

PluginUpdateInfo PluginManager::checkPluginUpdateSilent(const Plugin& plugin) {
    PluginUpdateInfo info;
    
    if (plugin.type == "spigot") {
        info = checkSpigetAPI(plugin.resourceId);
    } else if (plugin.type == "modrinth") {
        info = checkModrinthAPI(plugin.projectSlug, plugin.mcVersion);
    } else if (plugin.type == "github") {
        info = checkGitHubAPI(plugin.resourceId);
    } else if (plugin.type == "manual") {
        info.error = "Manual plugins require manual updates";
        return info;
    } else {
        info.error = "Unknown plugin type: " + plugin.type;
        return info;
    }
    
    if (!info.error.empty()) {
        return info;
    }
    
    // check if update is needed
    string currentVersion = getStoredVersion(plugin.name);
    info.hasUpdate = isNewerVersion(currentVersion, info.version);
    
    return info;
}

/**
 * savePluginConfigs
 * @brief function to save the given plugin config
 * @param configFile string reference to the name of the confile file
 * @return boolean value of whether the save was successful or not
 */
bool PluginManager::savePluginConfigs(const string& configFile) {
    ofstream file(configFile);
    if (!file.is_open()) {
        logger.err(LANGF("error.failed_to_write", configFile));
        return false;
    }
    
    file << "{\n  \"plugins\": [\n";
    
    for (size_t i = 0; i < plugins.size(); ++i) {
        const Plugin& plugin = plugins[i];
        
        file << "    {\n";
        file << "      \"name\": \"" << JsonHelper::escapeString(plugin.name) << "\",\n";
        file << "      \"type\": \"" << plugin.type << "\",\n";
        
        if (!plugin.resourceId.empty()) {
            if (plugin.type == "github") {
                file << "      \"repository\": \"" << plugin.resourceId << "\",\n";
            } else {
                file << "      \"resourceId\": \"" << plugin.resourceId << "\",\n";
            }
        }
        
        if (!plugin.projectSlug.empty()) {
            file << "      \"projectSlug\": \"" << plugin.projectSlug << "\",\n";
        }
        
        if (!plugin.mcVersion.empty()) {
            file << "      \"mcVersion\": \"" << plugin.mcVersion << "\",\n";
        }
        
        file << "      \"filenamePattern\": \"" << plugin.filenamePattern << "\",\n";
        file << "      \"disableOnTest\": " << (plugin.disableOnTest ? "true" : "false") << ",\n";
        file << "      \"enabled\": true\n";
        file << "    }";
        
        if (i < plugins.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ],\n";
    file << "  \"settings\": {\n";
    file << "    \"defaultMinecraftVersion\": \"" << Config::getInstance().getMinecraftVersion() << "\",\n";
    file << "    \"autoDetectDisabled\": true,\n";
    file << "    \"backupBeforeUpdate\": true\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    return true;
}

/**
 * getStoredVersion
 * @brief function to get the saved version of a given plugin
 * @param pluginName string reference to the name of the plugin
 * @returns string of the plugin version
 */
string PluginManager::getStoredVersion(const string& pluginName) {
    auto it = storedVersions.find(pluginName);
    return (it != storedVersions.end()) ? it->second : "unknown";
}

/**
 * setStoredVersion
 * @brief function to set the stored version of a given plugin
 * @param pluginName string reference to the name of the plugin
 * @param version string reference to the version we want to set
 */
void PluginManager::setStoredVersion(const string& pluginName, const string& version) {
    storedVersions[pluginName] = version;
    savePluginVersions();
    logger.log(LANGF("plugin.version_updated", pluginName, version));
}

/**
 * checkSpigetAPI
 * @brief function to check a resource from the spiget api
 * @param resourceId string reference to the resource id for a plugin
 * @returns PluginUpdateInfo
 */
PluginUpdateInfo PluginManager::checkSpigetAPI(const string& resourceId) {
    PluginUpdateInfo info;
    
    // first get basic resource info
    string url = "https://api.spiget.org/v2/resources/" + resourceId;
    map<string, string> headers = {
        {"Accept", "application/json"},
        {"User-Agent", "Pluginator/1.0.3"}
    };
    
    HttpResponse response = httpClient.get(url, headers);
    
    if (!response.success) {
        info.error = "API request failed: " + response.error;
        return info;
    }
    
    string pluginName = JsonHelper::extractValue(response.body, "name");
    string externalUrl = JsonHelper::extractValue(response.body, "externalUrl");
    
    if (!externalUrl.empty()) {
        // external plugin
        regex versionRegex(R"(/v?([0-9]+\.[0-9]+(?:\.[0-9]+)?(?:-[A-Za-z0-9\-\.]+)?)/[^/]+\.jar)");
        smatch match;
        if (regex_search(externalUrl, match, versionRegex)) {
            info.version = match[1].str();
            info.downloadUrl = externalUrl;
        }
    } else {
        // internal plugin
        string versionsUrl = "https://api.spiget.org/v2/resources/" + resourceId + "/versions/latest";
        HttpResponse versionResponse = httpClient.get(versionsUrl, headers);
        
        if (versionResponse.success) {
            string version = JsonHelper::extractValue(versionResponse.body, "name");
            if (!version.empty()) {
                info.version = version;
            } else {
                // fallback to update id as version
                string updateId = JsonHelper::extractValue(versionResponse.body, "id");
                info.version = updateId.empty() ? "unknown" : updateId;
            }
        }
        
        string downloadPath = JsonHelper::extractValue(response.body, "url");
        if (!downloadPath.empty()) {
            info.downloadUrl = "https://api.spiget.org/v2/" + downloadPath;
        }
    }
    
    if (info.version.empty() || info.downloadUrl.empty()) {
        info.error = "Could not extract version or download URL";
        return info;
    }
    
    info.filename = pluginName + "-" + info.version + ".jar";
    info.hasUpdate = true;
    return info;
}

/**
 * checkModrinthAPI
 * @brief function to check a project from the modrinth api
 * @param projectSlug string reference to the slug of the project we want to check
 * @param mcVersion string reference to the minecraft version we want the plugin to be for
 * @returns PluginUpdateInfo
 */
PluginUpdateInfo PluginManager::checkModrinthAPI(const string& projectSlug, const string& mcVersion) {
    PluginUpdateInfo info;
    
    // priority order: purpur -> paper -> spigot -> bukkit
    vector<string> loaderPriority = {"purpur", "paper", "spigot", "bukkit"};
    
    for (const string& loader : loaderPriority) {
        string url = "https://api.modrinth.com/v2/project/" + projectSlug + "/version";
        url += "?game_versions=%5B%22" + mcVersion + "%22%5D"; // url encoded mc version
        url += "&loaders=%5B%22" + loader + "%22%5D"; // single loader per request
        
        map<string, string> headers = {
            {"Accept", "application/json"},
            {"User-Agent", "Pluginator"}
        };
        
        logger.debug("Modrinth API URL (" + loader + "): " + url);
        
        HttpResponse response = httpClient.get(url, headers);

        if (!response.success) {
            logger.debug("Loader " + loader + " failed: " + response.error);
            if (loader == loaderPriority.back()) {
                info.error = "All API requests failed: " + response.error;
                return info;
            }
            continue; // try next loader
        }

        if (response.body.empty()) {
            logger.debug("Empty response for loader: " + loader);
            continue;
        }
        
        // found a version with this loader - parse and return
        logger.debug("Found version with loader: " + loader);
        logger.debug("Modrinth Response: " + response.body.substr(0, 500));
        
        // extract version info
        string versionPattern = "\"version_number\"\\s*:\\s*\"([^\"]+)\"";
        string urlPattern = "\"url\"\\s*:\\s*\"([^\"]+)\""; 
        string filenamePattern = "\"filename\"\\s*:\\s*\"([^\"]+)\""; 
        
        regex versionRegex(versionPattern);
        regex urlRegex(urlPattern);
        regex filenameRegex(filenamePattern);
        
        smatch match;
        
        if (regex_search(response.body, match, versionRegex)) {
            info.version = match[1].str();
        }
        
        if (regex_search(response.body, match, urlRegex)) {
            info.downloadUrl = match[1].str();
        }
        
        if (regex_search(response.body, match, filenameRegex)) {
            info.filename = match[1].str();
        }
        
        if (!info.version.empty() && !info.downloadUrl.empty()) {
            info.hasUpdate = true;
            logger.debug("Successfully found version " + info.version + " for loader: " + loader);
            return info;
        }
    }
    
    // no compatible versions found for any loader
    info.error = "No versions found for MC " + mcVersion + " with compatible loaders";
    return info;
}

/**
 * checkSpigetAPI
 * @brief function to check a resource from github api
 * @param repo string reference to the project repository
 * @returns PluginUpdateInfo
 */
PluginUpdateInfo PluginManager::checkGitHubAPI(const string& repo) {
    PluginUpdateInfo info;
    
    string url = "https://api.github.com/repos/" + repo + "/releases/latest";
    map<string, string> headers = {
        {"Accept", "application/vnd.github.v3+json"},
        {"User-Agent", "Pluginator"}
    };
    
    HttpResponse response = httpClient.get(url, headers);
    
    if (!response.success) {
        info.error = "GitHub API request failed: " + response.error;
        return info;
    }
    
    // extract version and download url
    info.version = JsonHelper::extractValue(response.body, "tag_name");
    
    // remove 'v' prefix if present
    if (!info.version.empty() && info.version[0] == 'v') {
        info.version = info.version.substr(1);
    }
    
    // find .jar asset download URL
    regex assetPattern("\"browser_download_url\"\\s*:\\s*\"([^\"]*\\.jar)\"");
    smatch match;
    
    if (regex_search(response.body, match, assetPattern)) {
        info.downloadUrl = match[1].str();
        // extract filename from url
        size_t lastSlash = info.downloadUrl.find_last_of('/');
        if (lastSlash != string::npos) {
            info.filename = info.downloadUrl.substr(lastSlash + 1);
        }
    }
    
    if (info.version.empty()) {
        info.error = "Could not find version in GitHub release";
        return info;
    }
    
    if (info.downloadUrl.empty()) {
        info.error = "Could not find .jar download in GitHub release";
        return info;
    }
    
    info.hasUpdate = true;
    return info;
}

// @todo BUILD OUT THIS FUNCTION
PluginUpdateInfo PluginManager::checkJenkinsAPI(const string& jenkinsType, const string& job) {
    PluginUpdateInfo info;
    string baseUrl;
    
    // different jenkins server types
    if (jenkinsType == "froobworld") {
        baseUrl = "https://ci.froobworld.com";
    } else if (jenkinsType == "codemc") {
        baseUrl = "https://ci.codemc.io";
    } else if (jenkinsType == "enginehub") {
        baseUrl = "https://builds.enginehub.org";
    } else if (jenkinsType == "papermc") {
        baseUrl = "https://paperci.io";
    } else if (jenkinsType == "custom") {
        // for custom jenkins instances, job should contain full url
        baseUrl = "";
    } else {
        info.error = "Unknown Jenkins type: " + jenkinsType;
        return info;
    }
    
    string url;
    if (jenkinsType == "custom") {
        url = job; // full url
    } else {
        url = baseUrl + "/job/" + job + "/lastSuccessfulBuild/api/json";
    }
    
    map<string, string> headers = {
        {"Accept", "application/json"},
        {"User-Agent", "Pluginator"}
    };
    
    logger.debug("Jenkins API URL: " + url);
    
    HttpResponse response = httpClient.get(url, headers);
    
    if (!response.success) {
        info.error = "Jenkins API request failed: " + response.error;
        return info;
    }
    
    logger.debug("Jenkins Response: " + response.body.substr(0, 500));
    
    // extract build number as version
    string buildNumber = JsonHelper::extractValue(response.body, "number");
    if (buildNumber.empty()) {
        info.error = "Could not extract build number from Jenkins response";
        return info;
    }
    
    info.version = "build-" + buildNumber;
    
    // look for .jar artifacts
    regex artifactPattern("\"fileName\"\\s*:\\s*\"([^\"]*\\.jar)\"");
    regex relativePathPattern("\"relativePath\"\\s*:\\s*\"([^\"]*\\.jar)\"");
    
    smatch match;
    string jarFileName;
    string relativePath;
    
    if (regex_search(response.body, match, artifactPattern)) {
        jarFileName = match[1].str();
    }
    
    if (regex_search(response.body, match, relativePathPattern)) {
        relativePath = match[1].str();
    }
    
    if (jarFileName.empty()) {
        info.error = "No .jar artifacts found in Jenkins build";
        return info;
    }
    
    // construct download URL
    if (jenkinsType == "custom") {
        // for custom, try to construct artifact url from base url
        size_t apiPos = url.find("/api/json");
        if (apiPos != string::npos) {
            string buildUrl = url.substr(0, apiPos);
            info.downloadUrl = buildUrl + "/artifact/" + relativePath;
        } else {
            info.error = "Could not construct download URL for custom Jenkins";
            return info;
        }
    } else {
        info.downloadUrl = baseUrl + "/job/" + job + "/lastSuccessfulBuild/artifact/" + relativePath;
    }
    
    info.filename = jarFileName;
    info.hasUpdate = true;
    
    logger.debug("Jenkins version: " + info.version);
    logger.debug("Jenkins download URL: " + info.downloadUrl);
    
    return info;
}

/**
 * isNewerVersion
 * @brief function to check if we got a newer version of a plugin
 * @param current string reference to the current plugin we have
 * @param latest string reference to the latest plugin version available
 * @returns boolean value of whether there is a newer version or not
 */
bool PluginManager::isNewerVersion(const string& current, const string& latest) {
    if (current == "unknown" || current.empty()) return true;
    if (latest.empty()) return false;
    if (current == latest) return false;
    
    // simple version comparison
    auto parseVersion = [](const string& version) -> vector<int> {
        vector<int> parts;
        stringstream ss(version);
        string part;
        
        while (getline(ss, part, '.')) {
            // extract numeric part from version component
            regex numberRegex(R"(\d+)");
            smatch match;
            if (regex_search(part, match, numberRegex)) {
                parts.push_back(stoi(match[0].str()));
            } else {
                parts.push_back(0);
            }
        }
        return parts;
    };
    
    vector<int> currentParts = parseVersion(current);
    vector<int> latestParts = parseVersion(latest);
    
    // pad shorter version with zeros
    size_t maxSize = max(currentParts.size(), latestParts.size());
    currentParts.resize(maxSize, 0);
    latestParts.resize(maxSize, 0);
    
    // compare version parts
    for (size_t i = 0; i < maxSize; ++i) {
        if (latestParts[i] > currentParts[i]) return true;
        if (latestParts[i] < currentParts[i]) return false;
    }
    
    return false; // versions are equal
}

PluginUpdateInfo PluginManager::checkPluginUpdate(const Plugin& plugin) {
    PluginUpdateInfo info;
    
    logger.debug(LANGF("plugin.checking", plugin.name));
    
    // check based on plugin type
    if (plugin.type == "spigot") {
        info = checkSpigetAPI(plugin.resourceId);
    } else if (plugin.type == "modrinth") {
        info = checkModrinthAPI(plugin.projectSlug, plugin.mcVersion);
    } else if (plugin.type == "github") {
        info = checkGitHubAPI(plugin.resourceId); // reusing resourceId
    } else if (plugin.type == "jenkins") {
        info = checkJenkinsAPI(plugin.jType, plugin.job);
    } else if (plugin.type == "manual") {
        info.error = "Manual plugins require manual updates";
        return info;
    } else {
        info.error = "Unknown plugin type: " + plugin.type;
        return info;
    }
    
    if (!info.error.empty()) {
        return info;
    }
    
    // check if update is needed
    string currentVersion = getStoredVersion(plugin.name);
    info.hasUpdate = isNewerVersion(currentVersion, info.version);
    
    return info;
}

void PluginManager::showDownloadProgress(double progress) {
    int barWidth = 50;
    cout << "[";
    int pos = barWidth * progress / 100.0;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << fixed << setprecision(1) << progress << "%\r";
    cout.flush();
}

bool PluginManager::downloadPlugin(const PluginUpdateInfo& updateInfo, const string& outputPath) {
    logger.log(LANGF("plugin.downloading", updateInfo.filename));
    logger.log(LANGF("plugin.download_path", outputPath));
    
    bool success = httpClient.downloadFile(updateInfo.downloadUrl, outputPath, nullptr);
    
    if (success) {
        // show file size after successful download
        ifstream file(outputPath, ios::binary | ios::ate);
        if (file.is_open()) {
            size_t fileSize = file.tellg();
            file.close();
            
            logger.success(LANGF("plugin.download_success_with_size", 
                                updateInfo.filename, 
                                Utils::formatBytes((int64_t)fileSize)));
        } else {
            logger.success(LANGF("plugin.download_success", updateInfo.filename));
        }
    } else {
        logger.err(LANGF("plugin.download_failed", updateInfo.filename));
    }
    
    return success;
}

void PluginManager::checkAllPluginUpdates(const vector<Plugin>& plugins, bool downloadUpdates) {
    logger.log(LANG("plugin.checking_all"));
    
    int updateCount = 0;
    int errorCount = 0;
    
    for (const Plugin& plugin : plugins) {
        cout << endl;
        
        PluginUpdateInfo updateInfo = checkPluginUpdate(plugin);
        string currentVersion = getStoredVersion(plugin.name);
        
        cout << "  " << Colors::BLUE << plugin.name << Colors::NC << endl;
        cout << "    Current: " << currentVersion << endl;
        
        if (!updateInfo.error.empty()) {
            cout << "    " << Colors::RED << "Error: " << updateInfo.error << Colors::NC << endl;
            errorCount++;
            continue;
        }
        
        cout << "    Latest:  " << updateInfo.version << endl;
        
        if (updateInfo.hasUpdate) {
            cout << "    " << Colors::YELLOW << LANG("plugin.update_available") << Colors::NC << endl;
            updateCount++;
            
            if (downloadUpdates) {
                string testServerPath = Config::getInstance().getTestServerPath();
                if (updatePlugin(plugin, testServerPath)) {
                    cout << "    " << Colors::GREEN << LANG("plugin.updated_successfully") << Colors::NC << endl;
                } else {
                    cout << "    " << Colors::RED << LANG("plugin.update_failed") << Colors::NC << endl;
                }
            }
        } else {
            cout << "    " << Colors::GREEN << LANG("plugin.up_to_date") << Colors::NC << endl;
        }
    }
    
    cout << endl;
    if (downloadUpdates) {
        logger.success(LANGF("plugin.update_summary_downloaded", to_string(updateCount)));
    } else {
        logger.log(LANGF("plugin.update_summary", to_string(updateCount), to_string(errorCount)));
    }
}

void PluginManager::showPluginStatus(const vector<Plugin>& plugins) {
    cout << endl << Colors::BLUE << "Plugin Status Report:" << Colors::NC << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    
    string testPluginsPath = Config::getInstance().getTestServerPath() + "/plugins";
    int totalPlugins = plugins.size();
    
    // struct to store results from single check
    struct PluginStatus {
        string currentVersion;
        string latestVersion;
        string statusText;
        string statusColor;
    };
    
    vector<PluginStatus> results;
    results.reserve(totalPlugins);
    
    ProgressBar progress(totalPlugins, "Checking plugins");
    
    // check each plugin once and store results
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        // do the actual checking
        PluginStatus status;
        status.currentVersion = getStoredVersion(plugin.name);
        status.statusColor = Colors::RED;
        status.statusText = "Missing";
        status.latestVersion = "N/A";
        
        // check if plugin exists
        bool pluginExists = false;
        if (exists(testPluginsPath)) {
            for (const auto& entry : directory_iterator(testPluginsPath)) {
                string filename = entry.path().filename().string();
                if (filename.find(plugin.name.substr(0, min(5, (int)plugin.name.length()))) != string::npos &&
                    endsWith(filename, ".jar") && !endsWith(filename, ".jar.DIS")) {
                    pluginExists = true;
                    break;
                }
            }
        }
        
        if (pluginExists) {
            if (status.currentVersion != "unknown") {
                status.statusColor = Colors::GREEN;
                status.statusText = "Installed";
                
                if (plugin.type != "manual") {
                    PluginUpdateInfo updateInfo = checkPluginUpdate(plugin);
                    
                    if (updateInfo.error.empty()) {
                        status.latestVersion = updateInfo.version;
                        if (updateInfo.hasUpdate) {
                            status.statusColor = Colors::YELLOW;
                            status.statusText = "Update Avail";
                        }
                    } else {
                        status.latestVersion = "API Error";
                    }
                } else {
                    status.latestVersion = "Manual";
                }
            } else {
                status.statusColor = Colors::YELLOW;
                status.statusText = "Untracked";
                status.latestVersion = "Unknown";
            }
        } else {
            // check if disabled
            bool isDisabled = false;
            if (exists(testPluginsPath)) {
                for (const auto& entry : directory_iterator(testPluginsPath)) {
                    string filename = entry.path().filename().string();
                    if (filename.find(plugin.name.substr(0, min(5, (int)plugin.name.length()))) != string::npos &&
                        endsWith(filename, ".jar.DIS")) {
                        isDisabled = true;
                        break;
                    }
                }
            }
            
            if (isDisabled) {
                status.statusColor = Colors::BLUE;
                status.statusText = "Disabled";
            }
        }
        
        results.push_back(status);
    }
    
    progress.finish();
    
    cout << " Complete!" << endl << endl;
    
    // display header
    cout << Colors::CYAN;
    printf("%-30s %-20s %-10s %-18s %-15s\n", 
           "PLUGIN NAME", "CURRENT VERSION", "TYPE", "LATEST VERSION", "STATUS");
    cout << Colors::NC;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    
    // display results
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        const PluginStatus& status = results[i];
        
        // truncate long strings for display
        string displayName = plugin.name.length() > 29 ? plugin.name.substr(0, 26) + "..." : plugin.name;
        string displayCurrentVer = status.currentVersion.length() > 19 ? status.currentVersion.substr(0, 16) + "..." : status.currentVersion;
        string displayLatestVer = status.latestVersion.length() > 17 ? status.latestVersion.substr(0, 14) + "..." : status.latestVersion;
        
        printf("%-30s %-20s %-10s %-18s ", 
               displayName.c_str(),
               displayCurrentVer.c_str(),
               plugin.type.c_str(),
               displayLatestVer.c_str());
        
        printf("%s%-15s%s\n", status.statusColor.c_str(), status.statusText.c_str(), Colors::NC.c_str());
    }
    
    // show summary info
    int totalConfigured = plugins.size();
    int totalTracked = storedVersions.size();
    int installedCount = 0;
    int updateAvailableCount = 0;
    
    // count from already-stored results
    for (const PluginStatus& status : results) {
        if (status.statusText == "Installed" || status.statusText == "Update Avail") {
            installedCount++;
        }
        
        if (status.statusText == "Update Avail") {
            updateAvailableCount++;
        }
    }
    
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;

    // summary section
    cout << endl << Colors::BLUE << "Summary:" << Colors::NC << endl;
    printf("  • Total configured plugins: %d\n", totalPlugins);
    printf("  • Tracked versions: %d\n", (int)storedVersions.size());
    printf("  • Installed plugins: %d\n", installedCount);

    if (updateAvailableCount > 0) {
        printf("  • %sUpdates available: %d%s\n", 
            Colors::YELLOW.c_str(), updateAvailableCount, Colors::NC.c_str());
    } else {
        printf("  • %sAll plugins up to date%s\n", 
            Colors::GREEN.c_str(), Colors::NC.c_str());
    }

    cout << "  • Plugin versions file: " << pluginVersionsFile << endl;
    cout << "  • Test server plugins: " << testPluginsPath << endl;

    
    // show what files are actually in the plugins directory
    if (logger.isDebugEnabled()) {
        cout << endl << Colors::BLUE << "Files in test server plugins directory:" << Colors::NC << endl;
        if (exists(testPluginsPath)) {
            vector<string> jarFiles;
            for (const auto& entry : directory_iterator(testPluginsPath)) {
                if (endsWith(entry.path().filename().string(), ".jar") || 
                    endsWith(entry.path().filename().string(), ".jar.DIS")) {
                    jarFiles.push_back(entry.path().filename().string());
                }
            }
            
            if (!jarFiles.empty()) {
                sort(jarFiles.begin(), jarFiles.end());
                for (const string& file : jarFiles) {
                    if (endsWith(file, ".jar.DIS")) {
                        cout << "  " << Colors::BLUE << file << " (disabled)" << Colors::NC << endl;
                    } else {
                        cout << "  " << file << endl;
                    }
                }
            } else {
                cout << "  No .jar files found" << endl;
            }
        } else {
            cout << "  Directory not found: " << testPluginsPath << endl;
        }
    }
    
    cout << endl;
}

bool PluginManager::updatePlugin(const Plugin& plugin, const string& testServerPath) {
    PluginUpdateInfo updateInfo = checkPluginUpdate(plugin);
    
    if (!updateInfo.error.empty()) {
        logger.err(LANGF("plugin.check_failed", plugin.name, updateInfo.error));
        return false;
    }
    
    if (!updateInfo.hasUpdate) {
        logger.log(LANGF("plugin.already_updated", plugin.name));
        return true;
    }
    
    logger.log(LANGF("plugin.updating", plugin.name));
    
    // remove old plugin files
    string pluginsPath = testServerPath + "/plugins";
    if (exists(pluginsPath)) {
        for (const auto& entry : directory_iterator(pluginsPath)) {
            string filename = entry.path().filename().string();
            
            // simple pattern matching for plugin files
            if (filename.find(plugin.name.substr(0, min(5, (int)plugin.name.length()))) != string::npos &&
                filename.substr(filename.length() - 4) == ".jar") {
                try {
                    remove(entry.path());
                    logger.log(LANGF("plugin.old_removed", filename));
                } catch (const exception& e) {
                    logger.warn(LANGF("plugin.remove_failed", filename));
                }
            }
        }
    }
    
    // download new version
    string outputPath = pluginsPath + "/" + updateInfo.filename;
    if (downloadPlugin(updateInfo, outputPath)) {
        setStoredVersion(plugin.name, updateInfo.version);
        logger.success(LANGF("plugin.updated_to_version", plugin.name, updateInfo.version));
        return true;
    }
    
    return false;
}

bool PluginManager::downloadPluginToPath(const Plugin& plugin, const string& path) {
    PluginUpdateInfo updateInfo = checkPluginUpdate(plugin);
    
    if (!updateInfo.error.empty()) {
        logger.err(LANGF("plugin.check_failed", plugin.name, updateInfo.error));
        return false;
    }
    
    if (!updateInfo.hasUpdate) {
        logger.warn(LANGF("plugin.no_download_info", plugin.name));
        return false;
    }
    
    string outputPath = path + "/" + updateInfo.filename;
    return downloadPlugin(updateInfo, outputPath);
}