#include "../include/pluginator.hpp"
#include "version.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Pluginator C++ - Version " << PLUGINATOR_VERSION << endl;
    
    Pluginator app;
    
    if (argc > 1) {
        string command = argv[1];
        
        if (command == "sync") {
            app.sync();
        } else if (command == "backup-prod") {
            app.backup("./prod", "production_manual");
        } else if (command == "backup-test") {
            app.backup("./test", "test_manual");
        } else if (command == "logs") {
            // access logger through a public method
            cout << "Logs feature - use interactive mode for now" << endl;
        } else {
            cout << "Unknown command: " << command << endl;
            cout << "Available commands: sync, backup-prod, backup-test, logs" << endl;
        }
    } else {
        app.runInteractive();
    }
    
    return 0;
}