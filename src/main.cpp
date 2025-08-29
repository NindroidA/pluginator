#include "../include/pluginator.hpp"
#include "version.hpp"
#include <iostream>
#include <string>

using namespace std;

void showHelp() {
    cout << "Pluginator C++ - Version " << PLUGINATOR_VERSION << endl;
    cout << "\nUsage: pluginator [command]" << endl;
    cout << "\nCommands:" << endl;
    cout << "  sync              Sync plugins from production to test" << endl;
    cout << "  backup-prod       Create manual backup of production server" << endl;
    cout << "  backup-test       Create manual backup of test server" << endl;
    cout << "  migrate           Migrate test server to production (with backup)" << endl;
    cout << "  check-plugins     Check for plugin updates" << endl;
    cout << "  update-plugins    Download and install plugin updates" << endl;
    cout << "  plugin-status     Show current plugin status" << endl;
    cout << "  scan-versions     Scan and update saved plugin versions" << endl;
    cout << "  scan-versions-api Scan and update saved plugin versions via respective API" << endl;
    cout << "  verify-versions   Verify saved plugin versions" << endl;
    cout << "  check-purpur      Check for Purpur server updates" << endl;
    cout << "  logs              View recent log files" << endl;
    cout << "  backups           View recent backup files" << endl;
    cout << "  config            Show current configuration" << endl;
    cout << "  help              Show this help message" << endl;
    cout << "\nRun without arguments for interactive mode." << endl;
}


int main(int argc, char* argv[]) {
    // check for debug flag
    bool debugMode = false;
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--debug") {
            debugMode = true;
            break;
        }
    }

    if (debugMode) {
        setenv("PLUGINATOR_DEBUG", "1", 1);
    }

    if (argc == 1) {
        Pluginator app;
        app.runInteractive();
        return 0;
    }
    
    string command = argv[1];
    
    if (command == "help" || command == "-h" || command == "--help") {
        showHelp();
        return 0;
    }
    
    // init app for command mode
    Pluginator app;
    
    if (command == "sync") {
        cout << "🔄 Syncing plugins from production to test..." << endl;
        app.sync();
        
    } else if (command == "backup-prod") {
        cout << "💾 Creating production server backup..." << endl;
        app.backupProduction();
        
    } else if (command == "backup-test") {
        cout << "💾 Creating test server backup..." << endl;
        app.backupTest();
        
    } else if (command == "migrate") {
        cout << "🚚 Starting migration from test to production..." << endl;
        app.migrateToProduction();
        
    } else if (command == "check-plugins") {
        cout << "🔍 Checking plugin updates..." << endl;
        app.checkPluginUpdates();
        
    } else if (command == "update-plugins") {
        cout << "⬇️ Downloading plugin updates..." << endl;
        app.downloadPluginUpdates();
        
    } else if (command == "plugin-status") {
        cout << "📋 Showing plugin status..." << endl;
        app.showPluginUpdates();

    } else if (command == "scan-versions") {
        cout << "🔍 Scanning and Updating Plugin Versions..." << endl;
        app.scanAndUpdateVersions();
        
    } else if (command == "scan-versions-api") {
        cout << "🔍 Scanning and Updating Plugin Versions via API..." << endl;
        app.scanVersionsAPI();
        
    } else if (command == "verify-versions") {
        cout << "📋 Verifying Plugin Versions..." << endl;
        app.verifyVersions();

    } else if (command == "check-purpur") {
        cout << "🎯 Checking Purpur updates..." << endl;
        app.checkPurpur();
        
    } else if (command == "backups") {
        cout << "📋 Viewing recent backups..." << endl;
        app.viewRecentBackups();
        
    } else if (command == "config") {
        cout << "⚙️ Current configuration:" << endl;
        app.showConfiguration();
        
    } else {
        cout << "❌ Unknown command: " << command << endl;
        cout << "Run 'pluginator help' for available commands." << endl;
        return 1;
    }
    
    return 0;
}