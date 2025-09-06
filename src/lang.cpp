#include "../include/lang.hpp"
#include "../include/logger.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

Lang* Lang::instance = nullptr;

Lang::Lang() : currentLanguage("en") {
    // get the lang file location based on where the program build is running
    loadFromJson(Utils::getLangPath("en.json"));
}

/**
 * getInstance
 * @brief get the current lang instance
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
 * @brief function to load lang strings from a nested JSON file
 * @param filename string reference to the name of the file
 */
/**
 * loadFromJson
 * @brief function to load lang strings from a flat JSON file
 * @param filename string reference to the name of the file
 */
void Lang::loadFromJson(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[LANG] Warning: Could not open language file: " << filename << endl;
        return;
    }

    // clear existing strings
    strings.clear();
    
    string line;
    int lineNumber = 0;
    
    while (getline(file, line)) {
        lineNumber++;
        
        // trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // skip empty lines, comments, and structural JSON
        if (line.empty() || 
            line.find("//") == 0 || 
            line.find("#") == 0 || 
            line == "{" || 
            line == "}") {
            continue;
        }
        
        // look for key-value pairs: "key": "value"
        size_t colonPos = line.find(":");
        if (colonPos == string::npos) {
            continue;
        }
        
        // extract key (everything before colon)
        string key = line.substr(0, colonPos);
        
        // clean key: remove quotes, spaces, tabs
        key = regex_replace(key, regex("^\\s*[\"']"), "");    // remove leading quotes/whitespace
        key = regex_replace(key, regex("[\"']\\s*$"), "");    // remove trailing quotes/whitespace
        
        // extract value (everything after colon)
        string value = line.substr(colonPos + 1);
        
        // clean value: remove quotes, trailing comma, spaces
        value = regex_replace(value, regex("^\\s*[\"']"), "");      // remove leading quotes/whitespace
        value = regex_replace(value, regex("[\"'],?\\s*$"), "");    // remove trailing quotes/comma/whitespace
        
        // unescape common JSON escape sequences
        value = regex_replace(value, regex("\\\\\""), "\"");  // \"
        value = regex_replace(value, regex("\\\\\\\\"), "\\"); // (double backslash)  
        value = regex_replace(value, regex("\\\\n"), "\n");   // \n
        value = regex_replace(value, regex("\\\\t"), "\t");   // \t
        
        // only add non-empty key-value pairs
        if (!key.empty() && !value.empty()) {
            strings[key] = value;
        }
    }
    
    file.close();
    
    // report results
    if (!strings.empty()) {
        cout << "[LANG] ✅ Loaded " << strings.size() << " language strings from " << filename << endl;
    } else {
        cerr << "[LANG] ⚠️ No valid strings found in " << filename << endl;
    }
}

/**
 * parseJsonObject
 * @brief parse a JSON object and flatten nested keys with dot notation
 * @param jsonStr the JSON string to parse
 * @param keyPrefix prefix for nested keys (e.g., "cmd" becomes "cmd.")
 */
void Lang::parseJsonObject(const string& jsonStr, const string& keyPrefix) {
    istringstream stream(jsonStr);
    string line;
    
    while (getline(stream, line)) {
        // trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // skip empty lines, comments, and braces
        if (line.empty() || line[0] == '/' || line[0] == '#' || line == "{" || line == "}") {
            continue;
        }
        
        // look for key-value pairs
        size_t colonPos = line.find(":");
        if (colonPos == string::npos) continue;
        
        // extract key
        string key = line.substr(0, colonPos);
        key = regex_replace(key, regex("^\\s*[\"']"), "");    // remove leading quotes/spaces
        key = regex_replace(key, regex("[\"']\\s*$"), "");    // remove trailing quotes/spaces
        
        if (key.empty()) continue;
        
        // extract value part after colon
        string valuePart = line.substr(colonPos + 1);
        valuePart.erase(0, valuePart.find_first_not_of(" \t"));
        
        // check if this line starts a nested object
        if (valuePart.find("{") != string::npos) {
            // this is a nested object - collect all lines until matching }
            string nestedContent = "";
            int braceCount = 0;
            
            // count braces in current line
            for (char c : valuePart) {
                if (c == '{') braceCount++;
                if (c == '}') braceCount--;
            }
            
            // if braces don't close on same line, collect more lines
            if (braceCount > 0) {
                string nestedLine;
                while (getline(stream, nestedLine) && braceCount > 0) {
                    nestedContent += nestedLine + "\n";
                    for (char c : nestedLine) {
                        if (c == '{') braceCount++;
                        if (c == '}') braceCount--;
                    }
                }
            }
            
            // recursively parse nested content
            string newPrefix = keyPrefix.empty() ? key : keyPrefix + "." + key;
            parseJsonObject(nestedContent, newPrefix);
            
        } else {
            // this is a simple string value
            string value = valuePart;
            
            // remove quotes and trailing comma
            value = regex_replace(value, regex("^\\s*[\"']"), "");
            value = regex_replace(value, regex("[\"'],?\\s*$"), "");
            
            // unescape common JSON escape sequences  
            value = regex_replace(value, regex("\\\\\""), "\"");
            value = regex_replace(value, regex("\\\\\\\\"), "\\");
            value = regex_replace(value, regex("\\\\n"), "\n");
            value = regex_replace(value, regex("\\\\t"), "\t");
            
            if (!key.empty() && !value.empty()) {
                string fullKey = keyPrefix.empty() ? key : keyPrefix + "." + key;
                strings[fullKey] = value;
            }
        }
    }
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

/**
 * formatSafe
 * @brief safer formatting function that handles variable argument counts
 * @param key string reference to the lang key
 * @param args vector of arguments to substitute
 * @returns safe formatted string
 */
string Lang::formatSafe(const string& key, const vector<string>& args) const {
    string str = get(key);
    
    // replace placeholders {0}, {1}, {2}, etc.
    for (size_t i = 0; i < args.size(); ++i) {
        string placeholder = "{" + to_string(i) + "}";
        size_t pos = 0;
        while ((pos = str.find(placeholder, pos)) != string::npos) {
            str.replace(pos, placeholder.length(), args[i]);
            pos += args[i].length();
        }
    }
    
    return str;
}

/**
 * hasKey
 * @brief check if a language key exists
 * @param key string reference to check
 * @returns boolean value of if the key exists
 */
bool Lang::hasKey(const string& key) const {
    return strings.find(key) != strings.end();
}

/**
 * getMissingKeys  
 * @brief check which required keys are missing from loaded strings
 * @param requiredKeys vector of keys that should exist
 * @returns vector string of missing keys
 */
vector<string> Lang::getMissingKeys(const vector<string>& requiredKeys) const {
    vector<string> missing;
    for (const string& key : requiredKeys) {
        if (!hasKey(key)) {
            missing.push_back(key);
        }
    }
    return missing;
}

/**
 * getLoadedStringCount
 * @brief get count of loaded language strings
 * @returns int number of loaded strings
 */
int Lang::getLoadedStringCount() const {
    return static_cast<int>(strings.size());
}

string Lang::operator[](const string& key) const {
    return get(key);
}

/**
 * setLanguage
 * @brief set the language of the program
 * @param lang string reference to the desired lang (short form)
 */
void Lang::setLanguage(const string& lang) {
    currentLanguage = lang;
    string filename = "lang/" + lang + ".json";
    loadFromJson(filename);
}

/**
 * reloadStrings
 * @brief reload lang json file
 */
void Lang::reloadStrings() {
    strings.clear();
    loadFromJson("lang/" + currentLanguage + ".json");
}

/**
 * printLoadedStrings
 * @brief print all the loaded strings in the program
 */
void Lang::printLoadedStrings() const {
    cout << "\n📋 Loaded Language Strings (" << currentLanguage << "):\n";
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    for (const auto& pair : strings) {
        cout << "  " << pair.first << ": " << pair.second << "\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
}