#include "../include/json_helper.hpp"
#include <regex>
#include <sstream>

/**
 * parseSimpleObject
 * @brief simple json parsing function
 * @param json string reference to the json file
 * @returns map of two strings (key and value)
 */
map<string, string> JsonHelper::parseSimpleObject(const string& json) {
    map<string, string> result;
    
    // regex parsing for basic json objects
    regex pairRegex("\"([^\"]+)\"\\s*:\\s*\"([^\"]*)\"");
    sregex_iterator iter(json.begin(), json.end(), pairRegex);
    sregex_iterator end;
    
    for (; iter != end; ++iter) {
        smatch match = *iter;
        if (match.size() == 3) {
            string key = match[1].str();
            string value = unescapeString(match[2].str());
            result[key] = value;
        }
    }
    
    // handle numeric values
    regex numericRegex("\"([^\"]+)\"\\s*:\\s*([0-9.]+)");
    sregex_iterator numIter(json.begin(), json.end(), numericRegex);
    
    for (; numIter != end; ++numIter) {
        smatch match = *numIter;
        if (match.size() == 3) {
            string key = match[1].str();
            string value = match[2].str();
            result[key] = value;
        }
    }
    
    return result;
}

/**
 * createObject
 * @brief simple json object creator function
 * @param data map reference of two strings (key and value)
 * @returns the json string
 */
string JsonHelper::createObject(const map<string, string>& data) {
    string json = "{";
    bool first = true;
    
    for (const auto& pair : data) {
        if (!first) json += ",";
        json += "\"" + escapeString(pair.first) + "\":\"" + escapeString(pair.second) + "\"";
        first = false;
    }
    
    json += "}";
    return json;
}

/**
 * createObjectPretty
 * @brief simple json object creator function BUT this one makes it look better lmao
 * @param data map reference of two strings (key and value)
 * @returns the json string
 */
string JsonHelper::createObjectPretty(const map<string, string>& data) {
    if (data.empty()) return "{}";
    
    string json = "{\n";
    auto it = data.begin();
    
    while (it != data.end()) {
        json += "  \"" + escapeString(it->first) + "\": \"" + escapeString(it->second) + "\"";
        
        ++it;
        if (it != data.end()) {
            json += ",";
        }
        json += "\n";
    }
    
    json += "}";
    return json;
}

/**
 * extractValue
 * @brief simple json value extrator function
 * @param json string reference to the json file
 * @param key string reference to the key we want the value of
 * @returns string value of the given key
 */
string JsonHelper::extractValue(const string& json, const string& key) {
    // try string value first
    regex stringRegex("\"" + key + "\"\\s*:\\s*\"([^\"]*?)\"");
    smatch match;
    
    if (regex_search(json, match, stringRegex)) {
        return unescapeString(match[1].str());
    }
    
    // try numeric value
    regex numericRegex("\"" + key + "\"\\s*:\\s*([0-9.]+)");
    if (regex_search(json, match, numericRegex)) {
        return match[1].str();
    }
    
    // try boolean/null values
    regex boolRegex("\"" + key + "\"\\s*:\\s*(true|false|null)");
    if (regex_search(json, match, boolRegex)) {
        return match[1].str();
    }
    
    return "";
}

/**
 * extractNestedValue
 * @brief simple extracting nested values function
 * @param json string reference to the json file
 * @param path string reference to the file path
 * @returns string value
 */
string JsonHelper::extractNestedValue(const string& json, const string& path) {
    vector<string> parts;
    stringstream ss(path);
    string part;
    
    while (getline(ss, part, '.')) {
        parts.push_back(part);
    }
    
    if (parts.empty()) return "";
    
    // just try the path as is
    string currentJson = json;
    
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        // find the nested object
        regex objectRegex("\"" + parts[i] + "\"\\s*:\\s*\\{([^}]+)\\}");
        smatch match;
        if (regex_search(currentJson, match, objectRegex)) {
            currentJson = "{" + match[1].str() + "}";
        } else {
            return "";
        }
    }
    
    return extractValue(currentJson, parts.back());
}

/**
 * isValidJson
 * @brief simple function to check if the inputted json has valid format
 * @param json string reference to the json file
 * @returns boolean expressing wether or not the json is valid
 */
bool JsonHelper::isValidJson(const string& json) {
    // basic json validation
    if (json.empty()) return false;
    
    // check for balanced braces/brackets
    int braceCount = 0;
    int bracketCount = 0;
    bool inString = false;
    bool escaped = false;
    
    for (char c : json) {
        if (escaped) {
            escaped = false;
            continue;
        }
        
        if (c == '\\' && inString) {
            escaped = true;
            continue;
        }
        
        if (c == '"') {
            inString = !inString;
            continue;
        }
        
        if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '[') bracketCount++;
            else if (c == ']') bracketCount--;
        }
    }
    
    return braceCount == 0 && bracketCount == 0 && !inString;
}

/* json string utils */
string JsonHelper::escapeString(const string& str) {
    string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

string JsonHelper::unescapeString(const string& str) {
    string unescaped;
    bool escaped = false;
    
    for (char c : str) {
        if (escaped) {
            switch (c) {
                case 'n': unescaped += '\n'; break;
                case 'r': unescaped += '\r'; break;
                case 't': unescaped += '\t'; break;
                case '\\': unescaped += '\\'; break;
                case '"': unescaped += '"'; break;
                default: unescaped += c; break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else {
            unescaped += c;
        }
    }
    
    return unescaped;
}