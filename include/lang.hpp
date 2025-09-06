#pragma once
#include <string>
#include <map>
#include <vector>

using namespace std;

class Lang {
    static Lang* instance;
    map<string, string> strings;
    string currentLanguage;

    Lang();
    void loadFromJson(const string& filename);
    void parseJsonObject(const string& jsonContent, const string& prefix);

public:
    static Lang& getInstance();

    // getter functions
    string get(const string& key, const string& defaultValue = "") const;
    string format(const string& key, const string& arg1) const;
    string format(const string& key, const string& arg1, const string& arg2) const;
    string format(const string& key, const string& arg1, const string& arg2, const string& arg3) const;
    string formatSafe(const string& key, const vector<string>& args) const;

    // validation
    bool hasKey(const string& key) const;
    vector<string> getMissingKeys(const vector<string>& requiredKeys) const;
    int getLoadedStringCount() const;

    // lang management
    void setLanguage(const string& lang);
    string getLanguage() const { return currentLanguage; }

    string operator[](const string& key) const;
    void reloadStrings();
    void printLoadedStrings() const;
};

#define LANG(key) Lang::getInstance().get(key)
#define LANGF(key, ...) Lang::getInstance().format(key, __VA_ARGS__)