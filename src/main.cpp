#include "../include/pluginator.hpp"
#include "../include/lang.hpp"
#include "version.hpp"
#include <iostream>
#include <string>

using namespace std;

void showHelp() {
    cout << LANGF("cmd.title", PLUGINATOR_VERSION) << endl;
    cout << LANG("cmd.usage") << endl;
    cout << LANG("cmd.commands") << endl;
    cout << "  sync              " << LANG("cmd.sync.descrp") << endl;
    cout << "  backup-prod       " << LANG("cmd.backup-prod.descrp") << endl;
    cout << "  backup-test       " << LANG("cmd.backup-test.descrp") << endl;
    cout << "  migrate           " << LANG("cmd.migrate.descrp") << endl;
    cout << "  check-plugins     " << LANG("cmd.check-plugins.descrp") << endl;
    cout << "  update-plugins    " << LANG("cmd.update-plugins.descrp") << endl;
    cout << "  plugin-status     " << LANG("cmd.plugin-status.descrp") << endl;
    cout << "  scan-versions     " << LANG("cmd.scan-versions.descrp") << endl;
    cout << "  scan-versions-api " << LANG("cmd.scan-versions-api.descrp") << endl;
    cout << "  verify-versions   " << LANG("cmd.verify-versions.descrp") << endl;
    cout << "  check-purpur      " << LANG("cmd.check-purpur.descrp") << endl;
    cout << "  logs              " << LANG("cmd.logs.descrp") << endl;
    cout << "  backups           " << LANG("cmd.backups.descrp") << endl;
    cout << "  config            " << LANG("cmd.config.descrp") << endl;
    cout << "  help              " << LANG("cmd.help.descrp") << endl;
    cout << LANG("cmd.run_no_args") << endl;
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
        cout << LANG("cmd.sync.run") << endl;
        app.sync();
        
    } else if (command == "backup-prod") {
        cout << LANG("cmd.backup-prod.run") << endl;
        app.backupProduction();
        
    } else if (command == "backup-test") {
        cout << LANG("cmd.backup-test.run") << endl;
        app.backupTest();
        
    } else if (command == "migrate") {
        cout << LANG("cmd.migrate.run") << endl;
        app.migrateToProduction();
        
    } else if (command == "check-plugins") {
        cout << LANG("cmd.check-plugins.run") << endl;
        app.checkPluginUpdates();
        
    } else if (command == "update-plugins") {
        cout << LANG("cmd.update-plugins.run") << endl;
        app.downloadPluginUpdates();
        
    } else if (command == "plugin-status") {
        cout << LANG("cmd.plugin-status.run") << endl;
        app.showPluginUpdates();

    } else if (command == "scan-versions") {
        cout << LANG("cmd.scan-versions.run") << endl;
        app.scanAndUpdateVersions();
        
    } else if (command == "scan-versions-api") {
        cout << LANG("cmd.scan-versions-api.run") << endl;
        app.scanVersionsAPI();
        
    } else if (command == "verify-versions") {
        cout << LANG("cmd.verify-versions.run") << endl;
        app.verifyVersions();

    } else if (command == "check-purpur") {
        cout << LANG("cmd.check-purpur.run") << endl;
        app.checkPurpur();

    } else if (command == "logs") {
        cout << LANG("cmd.logs.run") << endl;
        app.viewRecentLogs();
        
    } else if (command == "backups") {
        cout << LANG("cmd.backups.run") << endl;
        app.viewRecentBackups();
        
    } else if (command == "config") {
        cout << LANG("cmd.config.run") << endl;
        app.showConfiguration();
        
    } else {
        cout << LANGF("cmd.unknown.descrp", command) << endl;
        cout << LANG("cmd.unknown.subdescrp") << endl;
        return 1;
    }
    
    return 0;
}