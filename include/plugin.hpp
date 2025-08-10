#pragma once
#include <string>
#include <map>

using namespace std;

struct Plugin {
    string name;
    string type;        // "spigot", "modrinth", "github", "manual", "other"
    string version;
    string resourceId;  // spigot/spiget
    string projectSlug; // modrinth
    string filenamePattern;
    string mcVersion;
    bool disableOnTest; // should this plugin be disabled when in the test server

    Plugin(string n, string t) : name(n), type(t), disableOnTest(false) {}
};