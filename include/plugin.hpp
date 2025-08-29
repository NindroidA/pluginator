#pragma once
#include <string>
#include <map>

using namespace std;

struct Plugin {
    string name;
    string type;        // "spigot", "modrinth", "github", "jenkins", "manual", "other"
    string jType;       // jenkins type (for url)
    string version;
    string resourceId;  // spigot/spiget
    string projectSlug; // modrinth
    string job;         // jenkins job name
    string mcVersion;
    string filenamePattern;
    bool disableOnTest; // should this plugin be disabled when in the test server

    Plugin(string n, string t) : name(n), type(t), disableOnTest(false) {}
};