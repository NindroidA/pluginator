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
    httpClient.setUserAgent("Pluginator");
    httpClient.setTimeout(30);
    loadTypeCache();
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
    logger.debug(LANGF("plugin.extract_version", filename));
    
    // remove .jar extension
    string name = filename;
    if (endsWith(name, ".jar")) {
        name = name.substr(0, name.length() - 4);
    }
    
    logger.debug(LANGF("plugin.without_jar", name));
    
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
            logger.debug(LANGF("plugin.pattern.matched", description, version));
            return version;
        }
    }
    
    logger.debug(LANG("plugin.pattern.no"));
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
            logger.debug(LANGF("plugin.disabled.skipping", pluginData["name"]));
            continue;
        }
        
        Plugin plugin = parsePluginFromJson(pluginData);
        if (!plugin.name.empty()) {
            plugins.push_back(plugin);
            loadedCount++;
            logger.debug(LANGF("plugin.loaded", plugin.name, plugin.type));
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
    plugin.mcVersion = getValue("mcVersion", Config::getInstance().getMinecraftVersion());
    
    plugin.disableOnTest = JsonHelper::parseBooleanValue(getValue("disableOnTest"));
    
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
    logger.debug(LANGF("plugin.config.init", configFile));

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
 * @brief function to scan all the jar files in the specified plugins directory with progress indication
 * @param pluginsPath string reference to the path where the plugins are stored
 */
void PluginManager::scanAndConfigurePlugins(const string& pluginsPath) {
    logger.log(LANGF("plugin.scanning", pluginsPath));
    
    if (!exists(pluginsPath)) {
        logger.err(LANGF("error.directory_not_found", pluginsPath));
        return;
    }
    
    // load existing type cache
    loadTypeCache();
    
    // collect all jar files first to show accurate progress
    vector<string> jarFiles;
    for (const auto& entry : directory_iterator(pluginsPath)) {
        if (!entry.is_regular_file()) continue;
        
        string filename = entry.path().filename().string();
        if (!endsWith(filename, ".jar") || endsWith(filename, ".jar.DIS")) continue;
        
        jarFiles.push_back(filename);
    }
    
    if (jarFiles.empty()) {
        logger.warn(LANGF("plugin.no_jars", pluginsPath));
        return;
    }
    
    vector<Plugin> detectedPlugins;
    
    // create progress bar for scanning
    ProgressBar progressBar(static_cast<int>(jarFiles.size()), "Scanning Plugins", 50);
    
    for (size_t i = 0; i < jarFiles.size(); ++i) {
        const string& filename = jarFiles[i];
        
        // extract plugin name from filename
        string pluginName = filename;
        
        // remove version numbers and .jar extension
        regex versionPattern(R"(-[0-9\\.]+\\.jar$)");
        pluginName = regex_replace(pluginName, versionPattern, "");
        
        // clean up common patterns
        pluginName = regex_replace(pluginName, regex("[-_]"), " ");
        
        // update progress bar with current plugin
        progressBar.update(static_cast<int>(i), pluginName);
        
        // detect plugin type (this may involve API calls)
        string detectedType = guessPluginType(pluginName);
        
        // create new plugin object
        Plugin plugin(pluginName, detectedType);
        plugin.filenamePattern = pluginName + "*.jar";
        
        detectedPlugins.push_back(plugin);
        logger.debug(LANGF("plugin.detected", pluginName, plugin.type));
    }
    
    // finish progress bar
    progressBar.finish("Scan completed!");
    
    logger.success(LANGF("plugin.scan_completed", to_string(detectedPlugins.size())));
    
    // show summary of detected types
    map<string, int> typeCounts;
    for (const Plugin& plugin : detectedPlugins) {
        typeCounts[plugin.type]++;
    }
    
    logger.log(LANG("plugin.type.detected"));
    for (const auto& typeCount : typeCounts) {
        logger.log("  " + typeCount.first + ": " + to_string(typeCount.second));
    }
    
    // offer to save configuration
    string response = logger.prompt(LANG("plugin.save_config_prompt"));
    if (response == "y" || response == "Y") {
        plugins = detectedPlugins;
        savePluginConfigs();
        logger.success(LANG("plugin.config.saved"));
    }
}

void PluginManager::scanAndUpdateVersions(const string& pluginsPath) {
    logger.log(LANG("plugin.scan.update_run"));
    
    if (!exists(pluginsPath)) {
        logger.err(LANGF("error.dir_not_found", pluginsPath));
        return;
    }
    
    int totalPlugins = plugins.size();
    int updatedCount = 0;
    
    ProgressBar progress(totalPlugins, LANG("plugin.scan.files"));
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string detectedVersion = detectPluginVersion(plugin, pluginsPath);
        
        if (!detectedVersion.empty() && detectedVersion != "unknown") {
            string currentStored = getStoredVersion(plugin.name);
            
            if (currentStored != detectedVersion) {
                setStoredVersion(plugin.name, detectedVersion);
                logger.debug(LANGF("plugin.scan.updated", plugin.name, currentStored, detectedVersion));
                updatedCount++;
            }
        } else {
            logger.debug(LANGF("plugin.scan.no_version", plugin.name));
        }
    }
    
    progress.finish(LANGF("plugin.version.summary", to_string(updatedCount)));
}

void PluginManager::scanAndUpdateVersionsWithAPI(const string& pluginsPath) {
    logger.log(LANG("plugin.version.api.run"));
    
    if (!exists(pluginsPath)) {
        logger.err(LANGF("error.dir_not_found", pluginsPath));
        return;
    }
    
    int totalPlugins = plugins.size();
    int updatedCount = 0;
    int apiUpdatedCount = 0;
    
    ProgressBar progress(totalPlugins, LANG("plugin.scan.files"));
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string detectedVersion = detectPluginVersion(plugin, pluginsPath);
        string currentStored = getStoredVersion(plugin.name);
        
        if (!detectedVersion.empty() && detectedVersion != "unknown") {
            if (currentStored != detectedVersion) {
                setStoredVersion(plugin.name, detectedVersion);
                logger.debug(LANGF("plugin.version.api.updated_filename", plugin.name, detectedVersion));
                updatedCount++;
            }
        } else {
            if (plugin.type != "manual") {
                logger.debug(LANGF("plugin.version.api.no_filename", plugin.name));
                
                PluginUpdateInfo apiInfo = checkPluginUpdate(plugin);
                
                if (!apiInfo.error.empty()) {
                    logger.debug(LANGF("plugin.version.api.failed", plugin.name, apiInfo.error));
                    continue;
                }
                
                if (!apiInfo.version.empty() && apiInfo.version != "unknown" && 
                    apiInfo.version != "spiget-latest" && apiInfo.version != "latest") {
                    
                    if (currentStored != apiInfo.version) {
                        setStoredVersion(plugin.name, apiInfo.version);
                        logger.debug(LANGF("plugin.version.api.updated_api", plugin.name, apiInfo.version));
                        apiUpdatedCount++;
                    }
                }
            }
        }
    }
    
    progress.finish(LANGF("plugin.version.api.updated_summary", to_string(updatedCount), to_string(apiUpdatedCount)));
}

void PluginManager::verifyVersionsWithAPI(const string& pluginsPath) {
    logger.log(LANG("plugin.verify.run"));
    
    int totalPlugins = plugins.size();
    int verifiedCount = 0;
    int correctedCount = 0;
    int newerAvailableCount = 0;
    
    ProgressBar progress(totalPlugins, LANG("plugin.verify.progress"));
    
    for (int i = 0; i < totalPlugins; i++) {
        const Plugin& plugin = plugins[i];
        
        progress.update(i + 1, plugin.name);
        
        string actualInstalledVersion = detectPluginVersion(plugin, pluginsPath);
        string storedVersion = getStoredVersion(plugin.name);
        
        if (actualInstalledVersion.empty() || actualInstalledVersion == "unknown") {
            logger.debug(LANGF("plugin.scan.no_version", plugin.name));
            continue;
        }
        
        if (storedVersion == actualInstalledVersion) {
            verifiedCount++;
            logger.debug(LANGF("plugin.verify.verified", plugin.name, storedVersion));
        } else {
            setStoredVersion(plugin.name, actualInstalledVersion);
            logger.debug(LANGF("plugin.verify.corrected", plugin.name, storedVersion, actualInstalledVersion));
            correctedCount++;
        }
        
        // check for updates
        if (plugin.type != "manual") {
            PluginUpdateInfo apiInfo = checkPluginUpdate(plugin);
            
            if (!apiInfo.error.empty()) {
                logger.debug(LANGF("plugin.version.api.failed ", plugin.name, apiInfo.error));
                continue;
            }
            
            if (!apiInfo.version.empty() && apiInfo.version != "unknown" && 
                apiInfo.version != "spiget-latest" && apiInfo.version != "latest") {
                
                if (isNewerVersion(actualInstalledVersion, apiInfo.version)) {
                    logger.debug(LANGF("plugin.update.api_available ", plugin.name, actualInstalledVersion, apiInfo.version));
                    newerAvailableCount++;
                }
            }
        }
    }
    
    progress.finish(LANGF("plugin.verify.summary", to_string(verifiedCount), to_string(correctedCount), to_string(newerAvailableCount)));
    logger.success(LANGF("plugin.verify.complete", to_string(verifiedCount), to_string(correctedCount), to_string(newerAvailableCount)));
}

string PluginManager::detectPluginVersion(const Plugin& plugin, const string& pluginsPath) {
    logger.debug(LANGF("plugin.version.detect.run", plugin.name));
    
    string foundFile = findMatchingPluginFile(plugin.name, pluginsPath);
    
    if (foundFile.empty()) {
        logger.debug(LANGF("plugin.version.detect.no_match", plugin.name));
        return "unknown";
    }
    
    logger.debug(LANGF("plugin.version.detect.match", foundFile));
    string version = extractVersionFromFilename(foundFile);
    logger.debug(LANGF("plugin.version.detect.extract", version));
    
    return version;
}

string PluginManager::findMatchingPluginFile(const string& pluginName, const string& pluginsPath) {
    vector<string> candidates = generatePluginNameVariants(pluginName);
    
    for (const auto& entry : directory_iterator(pluginsPath)) {
        string filename = entry.path().filename().string();
        
        if (!endsWith(filename, ".jar") || endsWith(filename, ".jar.DIS")) continue;
        
        logger.debug(LANGF("plugin.find.check", filename));
        
        if (isPluginMatch(candidates, filename)) {
            logger.debug(LANGF("plugin.find.match", filename));
            return filename;
        }
    }
    
    return "";
}

vector<string> PluginManager::generatePluginNameVariants(const string& pluginName) {
    string lower = pluginName;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    vector<string> variants = { lower };
    
    // handle specific known problem cases first
    if (lower == "evenmorefish") variants.push_back("even-more-fish");
    if (lower == "naruseapi") variants.push_back("naruseconfigapi");
    if (lower == "secondthreadapi") variants.push_back("narusesecondthreadapi"); 
    if (lower == "votifier") variants.push_back("nuvotifier");
    
    // case conversions
    string hyphenated = "";
    for (size_t i = 0; i < pluginName.length(); ++i) {
        char current = pluginName[i];
        
        // add hyphen before uppercase letters (except first character)
        if (i > 0 && isupper(current)) {
            // look ahead to avoid breaking acronyms like "API"
            bool isStartOfWord = true;
            if (i + 1 < pluginName.length() && isupper(pluginName[i + 1])) {
                isStartOfWord = false;
            }
            if (i > 0 && islower(pluginName[i - 1]) && isStartOfWord) {
                hyphenated += "-";
            }
        }
        hyphenated += tolower(current);
    }
    
    if (hyphenated != lower && hyphenated.find("--") == string::npos) {
        variants.push_back(hyphenated);
    }
    
    // remove common suffixes
    vector<string> suffixes = {"-reloaded", "v3", "v2", "-spigot", "-bukkit", "-paper"};
    for (const string& suffix : suffixes) {
        if (lower.length() > suffix.length() && 
            lower.substr(lower.length() - suffix.length()) == suffix) {
            variants.push_back(lower.substr(0, lower.length() - suffix.length()));
        }
    }
    
    return variants;
}

bool PluginManager::isPluginMatch(const vector<string>& variants, const string& filename) {
    string filenameLower = filename;
    transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);
    
    // remove jar extension
    string baseName = filenameLower.substr(0, filenameLower.length() - 4);
    
    // actual matching strategies
    for (const string& variant : variants) {
        // exact match
        if (baseName == variant) {
            return true;
        }
        
        // starts with variant followed by hyphen or version number
        if (baseName.find(variant + "-") == 0) {
            return true;
        }
        
        // handle cases like "evenmorefish" matching "even-more-fish"
        if (variant.length() >= 3 && baseName.find(variant) == 0) {
            // make sure not a false positive
            if (baseName.length() == variant.length() || 
                baseName[variant.length()] == '-' || 
                isdigit(baseName[variant.length()])) {
                return true;
            }
        }
    }
    
    return false;
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

PluginUpdateInfo PluginManager::checkPluginUpdateSilent(const Plugin& plugin) {
    PluginUpdateInfo info;
    
    if (plugin.type == "spigot") {
        info = checkSpigetAPI(plugin.resourceId);
    } else if (plugin.type == "modrinth") {
        info = checkModrinthAPI(plugin.projectSlug, plugin.mcVersion);
    } else if (plugin.type == "github") {
        info = checkGitHubAPI(plugin.resourceId);
    } else if (plugin.type == "manual") {
        info.error = LANG("plugin.info.updates");
        return info;
    } else {
        info.error = LANGF("plugin.info.unknown", plugin.type);
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

string PluginManager::guessPluginType(const string& pluginName) {
    // check cache first
    if (typeDetectionCache.find(pluginName) != typeDetectionCache.end()) {
        logger.debug(LANGF("plugin.guess.check", pluginName, typeDetectionCache[pluginName]));
        return typeDetectionCache[pluginName];
    }
    
    logger.debug(LANGF("plugin.guess.detect.run", pluginName));
    
    // normalize plugin name for API calls
    string normalized = pluginName;
    transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    // remove common suffixes that might interfere with API calls
    vector<string> suffixes = {"-reloaded", "v3", "v2", "-spigot", "-bukkit", "-paper", "-plugin"};
    for (const string& suffix : suffixes) {
        if (normalized.length() > suffix.length() && 
            normalized.substr(normalized.length() - suffix.length()) == suffix) {
            normalized = normalized.substr(0, normalized.length() - suffix.length());
            break;
        }
    }
    
    string detectedType = "manual"; // default
    
    // 1. try modrinth first (modern API, good search)
    string modrinthType = tryModrinthDetection(normalized);
    if (modrinthType != "unknown") {
        logger.debug(LANG("plugin.guess.found.modrinth"));
        detectedType = "modrinth";
    }
    // 2. try github (check common repo patterns)  
    else {
        string githubType = tryGitHubDetection(normalized);
        if (githubType != "unknown") {
            logger.debug(LANG("plugin.guess.found.github"));
            detectedType = "github";
        }
        // 3. try spigot as last resort
        else {
            string spigotType = trySpigotDetection(normalized);
            if (spigotType != "unknown") {
                logger.debug(LANG("plugin.guess.found.spigot"));
                detectedType = "spigot";
            }
        }
    }
    
    if (detectedType == "manual") {
        logger.debug(LANG("plugin.guess.no_found"));
    }
    
    // cache the result
    typeDetectionCache[pluginName] = detectedType;
    saveTypeCache();
    
    return detectedType;
}

void PluginManager::loadTypeCache() {
    string cacheFilePath = Utils::getDataPath(cacheFile);
    
    if (!exists(cacheFilePath)) return;
    
    ifstream file(cacheFilePath);
    if (!file.is_open()) return;
    
    string line;
    while (getline(file, line)) {
        size_t colonPos = line.find(":");
        if (colonPos != string::npos) {
            string key = line.substr(0, colonPos);
            string value = line.substr(colonPos + 1);
            
            // trim quotes
            key = regex_replace(key, regex("[\"']"), "");
            value = regex_replace(value, regex("[\"',]"), "");
            
            typeDetectionCache[key] = value;
        }
    }
    
    logger.debug(LANGF("plugin.cache.load", to_string(typeDetectionCache.size())));
}

void PluginManager::saveTypeCache() {
    string cacheFilePath = Utils::getDataPath(cacheFile);
    
    ofstream file(cacheFilePath);
    if (!file.is_open()) return;
    
    file << "{\n";
    bool first = true;
    for (const auto& pair : typeDetectionCache) {
        if (!first) file << ",\n";
        file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    file << "\n}\n";
    
    file.close();
}

string PluginManager::tryModrinthDetection(const string& pluginName) {
    logger.debug(LANGF("plugin.guess.modrinth.run", pluginName));
    
    string baseName = pluginName;
    transform(baseName.begin(), baseName.end(), baseName.begin(), ::tolower);
    
    // try common slug patterns
    vector<string> slugVariants = {
        baseName,
        regex_replace(baseName, regex("[ _]"), "-"),
        regex_replace(baseName, regex("([a-z])([A-Z])"), "$1-$2"),
    };
    
    // common modrinth naming patterns
    slugVariants.push_back(baseName + "-plugin");
    slugVariants.push_back(baseName + "-mod");
    
    for (const string& slug : slugVariants) {
        if (slug.empty()) continue;
        
        try {
            logger.debug(LANGF("plugin.guess.modrinth.slug", slug));
            PluginUpdateInfo info = checkModrinthAPI(slug, "1.21");
            
            // if we got a response without a 404 error, plugin exists
            if (info.error.find("404") == string::npos && info.error.find("Not Found") == string::npos) {
                if (!info.downloadUrl.empty() || !info.version.empty() || info.hasUpdate) {
                    logger.debug(LANGF("plugin.guess.modrinth.success", slug));
                    return "modrinth";
                }
            }
        } catch (const exception& e) {
            logger.debug(LANGF("api.call.failed", string(e.what())));
            continue;
        }
    }
    
    return "unknown";
}

string PluginManager::tryGitHubDetection(const string& pluginName) {
    logger.debug(LANGF("plugin.guess.github.run", pluginName));
    
    // patterns
    vector<string> repoPatterns = {
        pluginName + "/" + pluginName,                         // PluginName/PluginName
        "minecraft-" + pluginName + "/" + pluginName,          // minecraft-PluginName/PluginName  
        pluginName + "/minecraft-" + pluginName,               // PluginName/minecraft-PluginName
        pluginName + "/" + pluginName + "-plugin",             // PluginName/PluginName-plugin
        pluginName + "MC/" + pluginName,                       // PluginNameMC/PluginName
        "MC" + pluginName + "/" + pluginName,                  // MCPluginName/PluginName
    };
    
    // also try some popular minecraft plugin developers
    vector<string> knownDevelopers = {"ViaVersion"};
    for (const string& dev : knownDevelopers) {
        repoPatterns.push_back(dev + "/" + pluginName);
    }
    
    for (const string& repo : repoPatterns) {
        if (repo.empty() || repo.find("/") == string::npos) continue;
        
        try {
            logger.debug(LANGF("plugin.guess.github.repo", repo));
            PluginUpdateInfo info = checkGitHubAPI(repo);
            
            // if we got a response without a 404 error, repo exists
            if (info.error.find("404") == string::npos && info.error.find("Not Found") == string::npos) {
                if (!info.downloadUrl.empty() || !info.version.empty() || info.hasUpdate) {
                    logger.debug(LANGF("plugin.guess.github.repo", repo));
                    return "github";
                }
            }
        } catch (const exception& e) {
            logger.debug(LANGF("api.call.failed", string(e.what())));
            continue;
        }
    }
    
    return "unknown";
}

// @TODO: THIS ONES MORE DIFFICULT SO GOTTA THINK THONK ABOUT THIS
string PluginManager::trySpigotDetection(const string& pluginName) {
    logger.debug(LANGF("plugin.guess.spigot.run", pluginName));
    
    map<string, string> knownSpigotPlugins = {};
    
    string normalizedName = pluginName;
    transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::tolower);
    
    // check if we have a known resource ID
    if (knownSpigotPlugins.find(normalizedName) != knownSpigotPlugins.end()) {
        string resourceId = knownSpigotPlugins[normalizedName];
        
        try {
            logger.debug(LANGF("plugin.guess.spigot.resourceid", resourceId));
            PluginUpdateInfo info = checkSpigetAPI(resourceId);
            
            if (info.error.find("404") == string::npos && info.error.find("Not Found") == string::npos) {
                if (!info.downloadUrl.empty() || !info.version.empty() || info.hasUpdate) {
                    logger.debug(LANGF("plugin.guess.spigot.success", resourceId));
                    return "spigot";
                }
            }
        } catch (const exception& e) {
            logger.debug(LANGF("api.call.failed", string(e.what())));
        }
    }
    
    return "unknown";
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
        {"User-Agent", "Pluginator"}
    };
    
    HttpResponse response = httpClient.get(url, headers);
    
    if (!response.success) {
        info.error = LANGF("api.spiget.req_failed", response.error);
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
        info.error = LANG("api.spiget.url_failed");
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
        
        logger.debug(LANGF("api.modrinth.url", loader, url));
        
        HttpResponse response = httpClient.get(url, headers);

        if (!response.success) {
            logger.debug(LANGF("api.modrinth.loader_failed", loader, response.error));
            if (loader == loaderPriority.back()) {
                info.error = LANGF("api.modrinth.all_failed", response.error);
                return info;
            }
            continue; // try next loader
        }

        if (response.body.empty()) {
            logger.debug(LANGF("api.modrinth.loader_empty", loader));
            continue;
        }
        
        // found a version with this loader - parse and return
        logger.debug(LANGF("api.modrinth.loader_found", loader));
        logger.debug(LANGF("api.modrinth.res", response.body.substr(0, 500)));
        
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
            logger.debug(LANGF("api.modrinth.version_found", info.version, loader));
            return info;
        }
    }
    
    // no compatible versions found for any loader
    info.error = LANGF("api.modrinth.version_failed", mcVersion);
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
        info.error = LANGF("api.github.req_failed", response.error);
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
        info.error = LANG("api.github.no_version");
        return info;
    }
    
    if (info.downloadUrl.empty()) {
        info.error = LANG("api.github.no_jar");
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
        info.error = LANGF("api.jenkins.type_unknown", jenkinsType);
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
    
    logger.debug(LANGF("api.jenkins.url", url));
    
    HttpResponse response = httpClient.get(url, headers);
    
    if (!response.success) {
        info.error = LANGF("api.jenkins.req_failed", response.error);
        return info;
    }
    
    logger.debug(LANGF("api.jenkins.res", response.body.substr(0, 500)));
    
    // extract build number as version
    string buildNumber = JsonHelper::extractValue(response.body, "number");
    if (buildNumber.empty()) {
        info.error = LANG("api.jenkins.extract_failed");
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
        info.error = LANG("api.jenkins.no_jar");
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
            info.error = LANG("api.jenkins.url_failed");
            return info;
        }
    } else {
        info.downloadUrl = baseUrl + "/job/" + job + "/lastSuccessfulBuild/artifact/" + relativePath;
    }
    
    info.filename = jarFileName;
    info.hasUpdate = true;
    logger.debug(LANGF("api.jenkins.summary", info.version, info.downloadUrl));
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
        info.error = LANG("plugin.info.updates");
        return info;
    } else {
        info.error = LANGF("plugin.info.unknown", plugin.type);
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
            
            logger.success(LANGF("plugin.download_success_with_size", updateInfo.filename, Utils::formatBytes((int64_t)fileSize)));
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
    cout << endl << Colors::BLUE << LANG("plugin.status.title") << Colors::NC << endl;
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
    
    ProgressBar progress(totalPlugins, "Checking Plugins");
    
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
        bool pluginExists = !findMatchingPluginFile(plugin.name, testPluginsPath).empty();
        
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
                            status.statusText = "Update Available";
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
        if (status.statusText == "Installed" || status.statusText == "Update Available") {
            installedCount++;
        }
        
        if (status.statusText == "Update Available") {
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