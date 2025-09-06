#pragma once
#include <string>
#include <map>

using namespace std;

class JsonHelper {
public:
    // type helpers
    static bool parseBooleanValue(const string& value);
    static bool parseBooleanValue(int value);

    // json parsing for pairs
    static map<string, string> parseSimpleObject(const string& json);
    //static vector<map<string, string>> parseArray(const string& json);
    
    // json creation
    static string createObject(const map<string, string>& data);
    static string createObjectPretty(const map<string, string>& data);
    //static string createArray(const vector<map<string, string>>& data);
    
    // value extraction
    static string extractValue(const string& json, const string& key);
    static string extractNestedValue(const string& json, const string& path);
    
    // utils
    static bool isValidJson(const string& json);
    static string escapeString(const string& str);
    static string unescapeString(const string& str);
};