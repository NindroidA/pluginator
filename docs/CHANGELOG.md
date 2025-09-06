# Dev Update v1.2.1
- Reverted lang json to fully flat instead of having nested objects.
- Added boolean parse function in JsonHelper.
- Finally fixed all issues for detecting whether a plugin needs to be disabled or not (using the boolean parse function).
- Added more colors to Colors class.
- Changed progress bar colors.
- Added progress bar to the tar command when backing up files.
- Fixed plugins not getting re-disabled after migration command.
- Added INFO logger for sections with custom emojis n shit.
- Added quick build script.

# Dev Update v1.2.0
- Fixed plugin-status command logic (had old unuseful code that didn't do the checks correctly).
- ^ Also added more functions for plugin matching.
- Fixed file paths not working in my development environment.
- Fixed function that handles disabling plugins for the test server.
- Added handling for log file sizes.
- Improved logic for loading lang strings from json file.
- Added validation functions to Lang class.
- DEEP cleaned lang inconsistencies or hard-coded strings throughout the entire project.
- Attempted to add a function to parse nested json objects (it doesn't fully work).

# Dev Update v1.1.11
- Hotfix for lang problems and path problems.
- Idk what I was smoking when I started this project, but the generateConfigFromPluginList function in plugin_manager has gotta go (because it literally serves no purpose).

# Dev Update v1.1.10
- Hotfix for config error (cause i forgor to change the path).
- Removed unnecessary config files.

# Dev Update v1.1.9
- Hotfix for build error (cause i forgot two imports in logger lmao).

# Dev Update v1.1.8
- Cleaned up more small comments/unused tings.
- Will need to comment out more functions but that'll be for later
- First stable(ish) deploy (bugs will most likely be present).

# Dev Update v1.1.7
- Cleaned up file structure and organized different files instead of having them all in root dir.
- Added progress bar class and logic.
- Implemented progress bar across needed functions.

# Dev Update v1.1.6
- Added debug logger.
- Added debug flag for commands.
- Adding function for checking Jenkins API.
- Further fix of plugin status information.
- Added function for prettier json object creation.
- Added function for scanning plugin versions via file name.
- Added function for scanning plugin version via respective API.
- Added function for verifying plugin versions against installed files.

# Dev Update v1.1.5
- Further cleanup of comments and docs.
- Fixed purpur updating progress bar.
- Fixed plugin detection.
- Fixed plugin status table formatting.
- Fixed checking Modrinth API.
- Fixed checking Spiget API.

# Dev Update v1.1.4
- Added http_client logic.
- Added json_helper logic.
- Added utils header file.
- Further cleanup of functions (need to go back through and make sure there aren't any unused/forgotten shtuff).
- Added plugins.json file.

# Dev Update v1.1.3
- Added plugin_manager logic.
- Added purpur_manager logic.

# Dev Update v1.1.2
- Continued conversion of function logic.
- Added "lang" organization.
- Added lang logic.
- Added "config" organization (for env variables and such).
- Added config logic.

# Dev Update v1.1.1
- Continued conversion of function logic. 

# Dev Update v1.1.0
- Started migration from only using bash to C++ (original bash script will remain private unless I change my mind).
- At least listed all current functions (will add rest of logic later).
- File and directory organization for less clutter.

# Dev Update v1.0.3
- Added functions for log files.
- Added more functions for plugin info.
- Added functions for getting plugins from both Modrinth API and Spiget API.

# Dev Update v1.0.0-1.0.2
- Initial setup and organization of everything.