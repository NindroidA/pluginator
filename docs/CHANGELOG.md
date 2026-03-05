# Changelog

All notable changes to Pluginator will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.4.19] - 2026-03-05

### Fixed
- **Empty release notes**: Release notes now properly include changelog content for each version
- **Discord announcement timing**: Discord announcements now wait for docs to sync before sending, ensuring the changelog is up to date
- **Release artifacts missing**: Fixed an issue where release files could be lost during the build process

### Improved
- **Release pipeline organization**: Cleaner workflow structure with package managers grouped together

---

## [2.4.18] - 2026-03-05

### Fixed
- **Duplicate Discord announcements**: Release announcements no longer send twice

### Improved
- **Automatic release cleanup**: Old release binaries are now automatically pruned (keeping the latest 3), so the releases page stays clean

---

## [2.4.17] - 2026-03-05

### Fixed
- **4-digit version detection**: Plugins using 4+ segment versions (like CMI and CMILib) are now properly detected as having updates available

---

## [2.4.16] - 2026-03-05

### Fixed
- **Reliability improvements**: Fixed several edge cases in cache management, API URL validation, and navigation index handling identified during comprehensive code review
- **React best practices**: Moved side effects out of state updater functions in tab navigation

---

## [2.4.15] - 2026-03-05

### Improved
- **Expanded test coverage**: Added comprehensive tests for the web manifest plugin source and extracted navigation utilities for better testability

---

## [2.4.14] - 2026-03-04

### Improved
- **Expanded test coverage**: Added comprehensive tests for the Modrinth plugin source and HTTP client, covering API responses, error handling, caching, retry logic, and security validations

---

## [2.4.13] - 2026-03-04

### Improved
- **Internal code organization**: Consolidated UI overlay state management into a dedicated module, making the codebase easier to maintain and extend

---

## [2.4.12] - 2026-03-04

### Fixed
- **Smarter version comparison**: Pre-release versions (like SNAPSHOT, alpha, beta, rc) are now correctly identified as older than the full release. Build numbers (like -b131, -rev42) continue to be treated as equal to the base version.

---

## [2.4.11] - 2026-03-04

### Improved
- **Internal code cleanup**: Consolidated duplicate caching system into a single, more efficient cache with better statistics and smarter eviction. Cleaned up redundant code in the GitHub update checker.

---

## [2.4.10] - 2026-03-04

### Improved
- **Better keyboard experience**: Arrow key navigation stays active while data is loading, so you don't lose your position
- **Clearer home screen**: Update status display is more readable, and keyboard hints now show more shortcuts across two rows
- **Improved command search**: Commands like 'sources' and 'performance' are now easier to find with additional search keywords

---

## [2.4.9] - 2026-03-04

### Performance
- **Faster update checks**: Modrinth and Bukkit version filtering is now more efficient, and Spigot API calls run in parallel instead of one at a time
- **Smarter caching**: Source cache now keeps frequently-used entries longer (LRU eviction), and plugin JAR lookups are cached to avoid re-scanning directories

---

## [2.4.8] - 2026-03-04

### Quality
- **Expanded test coverage**: Added 40 new tests for critical security and infrastructure modules — API URL validation and server management now have comprehensive test suites

---

## [2.4.7] - 2026-03-04

### Improved
- **Better empty states**: Plugin list and dependency graph views now show helpful context-specific messages when there's nothing to display, guiding you to scan or configure
- **Stale cache warning**: When some update checks fail due to API errors and use cached results instead, you'll now see a clear notification so you know which results may be outdated

---

## [2.4.6] - 2026-03-04

### Improved
- **Config typo detection**: Pluginator now warns you if your config file has unrecognized keys and suggests the correct name — no more silently ignored settings
- **Safer downloads**: Plugin downloads now use atomic file writes — if something goes wrong mid-download, your existing plugin files stay intact
- **Extended security**: GeyserMC downloads now have the same URL safety checks as other sources

---

## [2.4.5] - 2026-03-04

### Security
- **Tighter tier enforcement**: Usage limits are now enforced locally when the server is unreachable, preventing bypass via network blocking
- **Hardened API connections**: All internal API connections are now validated against SSRF attacks — pointing Pluginator at a fake server no longer bypasses limits
- **Safer downloads**: Plugin download URLs from external sources are now validated before use, and download streams have a 500MB size cap

---

## [2.4.4] - 2026-03-04

### Added
- **More commands actually work**: `/clearfilters`, `/rollback`, `/exportcache`, and `/schedule:status` now do what they say instead of silently doing nothing
- **Stay logged in longer**: Your session now auto-refreshes every 12 hours while the app is running, so you won't randomly get logged out during long sessions

---

## [2.4.3] - 2026-03-04

### Fixed
- **Downloads actually work now**: After downloading plugin updates, the Updates tab no longer incorrectly shows them as still needing updates
- **Manual-only plugins don't get stuck**: Plugins that can only be downloaded manually no longer leave phantom selections in the updates list
- **Update command works**: Both the CLI `pluginator update` and the in-app `/update` command now actually install updates instead of just printing instructions

### Changed
- **Smoother gradient animation**: The premium text shimmer effect now uses a clean linear sweep instead of random spot placement — much smoother and more consistent across terminals
- **Stay logged in**: Your login session now persists across app restarts (previously logged you out after 24 hours even if the session was still valid)
- **Exit replaces Quit**: The command palette now says "Exit" instead of "Quit" (typing `/quit` still works)

---

## [2.4.2] - 2026-03-03

### Fixed
- **Update checks finally accurate**: After downloading a plugin update, the Updates tab now correctly shows it as up-to-date instead of still reporting it needs updating
- **Health score now reaches 100%**: Fixed health scores capping at 96.3 even when there are zero issues — plugins with no compatibility data are no longer penalized
- **Server JAR dialog display**: Fixed unicode bullet characters showing as raw text (`\u2022`) instead of rendering properly
- **Discord changelog announcements**: Changelog updates to Discord now trigger automatically when new versions are synced, fixing the empty announcement issue

---

## [2.4.1] - 2026-03-03

### Added
- **Auto-update on startup**: Pluginator now automatically checks for and installs updates before launching — no more manual update commands needed

### Fixed
- **Server JAR downloads**: Server JAR downloads now correctly go to your test server directory instead of production
- **Update checks**: The Updates tab now only shows plugins installed on your test server, keeping your production server untouched
- **Health score accuracy**: Fixed health scores showing 91 instead of 100 when there are no actual issues

---

## [2.4.0] - 2026-03-03

### Added
- **Per-server Minecraft versions**: You can now set separate Minecraft versions for your production and test servers (`PROD_MC_VERSION` and `TEST_MC_VERSION`). The old `MINECRAFT_VERSION` setting is automatically migrated
- **Config editor improvements**:
  - MC version fields now show a version picker instead of requiring manual text entry
  - Number fields support left/right arrow keys for quick increment/decrement
  - Each field shows a helpful description when selected

### Fixed
- **Auto-source command**: Running `/auto-source` multiple times no longer reports "sources updated" when sources already match — it correctly shows them as already assigned
- **Config editor reliability**: Fixed boolean toggle reading the wrong field value, and fixed options list mutation between sessions

---

## [2.3.20] - 2026-03-03

### Fixed
- **Version detection**: Fixed the updater always reporting version `0.0.0` instead of the actual installed version — update notifications now show the correct version
- **Plugin source persistence**: Fixed paid/premium plugins losing their "manual download" status after saving — plugins that should show in the "Manual Updates Available" section now stay correctly categorized

---

## [2.3.19] - 2026-03-03

### Changed
- **Better plugin sources**: Optimized download sources for 24 plugins — many plugins that previously relied on the unreliable Spiget API now use Modrinth, Hangar, or GitHub for faster, more reliable downloads
  - Fewer plugins should show as "manual download" when automatic updates are available
  - 2 previously unsourced plugins (PlayerHeads, LibsDisguises) now have download sources
  - All original Spigot sources kept as fallback — nothing removed, only improved

---

## [2.3.18] - 2026-03-02

### Fixed
- **Auto-updater broken**: Update checking now works reliably — switched from GitHub Releases API (which returned errors) to the npm registry where all versions are published
- **Version detection**: Fixed the updater reporting version `0.0.0` instead of the actual installed version in bundled builds
- **Update command**: Streamlined update instructions to show only the update command or download link

---

## [2.3.17] - 2026-03-02

### Added
- **Expanded plugin registry**: 104 curated plugins (up from 42) with verified sources across Spigot, Modrinth, GitHub, Jenkins, and Hangar
- **Manual download notifications**: Paid/premium plugins now get version checking — new "Manual Updates Available" section shows when updates are available with links to plugin pages
- **`/auto-source` command**: Re-assign all plugins to their recommended registry sources in one step
- **Dismissal cache**: Infrastructure for dismissing duplicate-file warnings with 24-hour expiry
- **Changelog shortcut**: Press `l` on the Updates tab to view a plugin's changelog

### Fixed
- **Disabled plugin display**: When both an enabled `.jar` and a disabled `.jar.disabled` exist, the enabled version is now correctly shown instead of the last-scanned version winning
- **Keyboard shortcut conflict**: Remapped `c` key across 7 views to prevent interference with Ctrl+C quit behavior — uses `l` (changelog), `x` (clear), `w` (collapse), `t` (category) depending on context
- **Premium plugin sources**: ItemsAdder, MythicMobs, and ShopGUI+ now correctly marked as manual-download to prevent failed auto-download attempts

### Changed
- **Status bar hints**: Updates tab now shows `l:changelog` instead of `c:changelog`
- **Help overlay**: Added `l` (changelog) to global keyboard shortcuts reference

---

## [2.3.16] - 2026-03-01

### Added
- **Disabled-state preservation**: Downloading an update for a disabled plugin now keeps it disabled instead of creating an enabled duplicate
- **Unrecognized file detection**: Sync and migrate now flag stale JAR remnants (`.jar.old`, `.jar.bak`, `.jar.backup`, `.jar.off`) in your plugins directory
- **Rollback disabled-file support**: Restoring from backup now correctly finds disabled plugin variants

### Fixed
- **Checksum verification**: Fixed a bug where checksum verification could fail after updating a disabled plugin
- **Server detection**: Running-server detection no longer false-positives on directories with similar names (e.g., `/mc` won't match `/mc-staging`)
- **Safe-mode migration rollback**: If a plugin is skipped or fails during safe-mode migration, the original file is restored instead of left disabled
- **Sync duplicate cleanup**: Stale duplicate JARs in the target directory are now cleaned up before copying

---

## [2.3.15] - 2026-02-28

### Fixed
- **Security hardening**: ReDoS protection on CLI regex input, SSRF validation on API URLs, JSON response size limits, download filename sanitization
- **Dead code cleanup**: Removed unused hooks, components, and utility functions
- **Expired tier handling**: Expired subscriptions now fall back to free-tier limits instead of blocking all actions
- **UI improvements**: Fixed empty states, help overlay key hints, config view labels, and keyboard shortcut hints

### Changed
- **Performance**: Optimized command palette sorting, log view search, and progress bar rendering

---

## [2.3.14] - 2026-02-28

### Fixed
- **55 code review findings** addressed across security, logic, and UX categories
- **278 new tests** added to cover fixed behaviors

---

## [2.3.12] - 2026-02-26

### Added
- **Directory browser in setup wizard**: Press Tab during path input to toggle a filesystem browser — navigate with arrow keys, Enter to select, Backspace to go up
- **`pluginator setup --headless`**: Plain-text Q&A setup mode for SSH, Docker, and small-terminal environments — no TUI required

---

## [2.3.11] - 2026-02-26

### Added
- **Headless environment support**: Automatically detects SSH, Docker, and display-less environments
- **PAT login for headless servers**: "Login with Access Token" shown as primary option on headless servers
- **`PLUGINATOR_API_TOKEN` env var**: Set this to authenticate automatically on startup (ideal for CI/Docker)
- **OAuth URL display**: When a browser can't be opened, the OAuth dialog shows the URL for manual copy-paste

---

## [2.3.10] - 2026-02-26

### Changed
- **Cross-platform color consistency**: All UI colors now use theme tokens instead of named ANSI colors, ensuring consistent appearance across macOS, Linux, and Windows terminals

---

## [2.3.9] - 2026-02-26

### Fixed
- **ReDoS protection**: Invalid regex patterns in `--grep` now fall back to literal matching instead of crashing
- **Download size limit**: HTTP downloads capped at 500 MB to prevent unbounded memory consumption
- **Health stability score**: Placeholder changed from misleading "100" to "50" to reflect unimplemented status

---

## [2.3.8] - 2026-02-25

### Added
- **Two-factor authentication**: CLI login now supports 2FA — if your account has TOTP enabled, you'll be prompted for your authenticator code after entering your password
- **`pluginator update` command**: Check for Pluginator updates from the command line (`pluginator update` or `/update` in the TUI)

### Fixed
- **Auto-updater**: The `/update` command in the TUI was not functional — now properly checks GitHub releases and shows update instructions

---

## [2.3.7] - 2026-02-24

### Added
- **Offline subscription detection**: Pluginator now detects expired subscriptions locally — no internet needed. If your subscription lapses, you're gracefully downgraded to your fallback tier
- **Headless server login**: New `pluginator login --token <token>` command lets you authenticate on CLI-only servers using a Personal Access Token (generate at nindroidsystems.com/account/tokens)
- **Device session tracking**: Your CLI now identifies itself to the API for concurrent session management
- **Session revocation**: If your session is ended from another device, Pluginator handles it cleanly

---

## [2.3.6] - 2026-02-23

### Added
- **ElytraRace** plugin added to the curated registry — competitive elytra racing with rings, timers, and leaderboards

---

## [2.3.5] - 2026-02-22

### Changed
- **Tier system restructured**: Limits are now per-operation (1 button click = 1 usage, regardless of how many plugins are affected)
  - Migrations now tracked separately from syncs
  - Updated limits across all tiers to reflect per-operation counting
- **Tier enforcement active**: Daily limits are now checked and enforced before operations
  - Warning notification shown when limit is reached
  - Works offline — allows all actions when API is unavailable

---

## [2.3.4] - 2026-02-21

### Added
- **GeyserMC Source**: Native support for downloading Geyser and Floodgate plugins directly from the GeyserMC download API
  - Geyser and Floodgate now use the official GeyserMC API as their preferred source
  - Hangar and Modrinth sources available as fallbacks

### Fixed
- Fixed package name in uninstall and update commands (v2.3.3)
- Fixed native binding crash on npm/bun installs (v2.3.2)

---

## [2.3.1] - 2026-02-21

### Fixed
- Fixed incorrect URLs, outdated paths, and inconsistencies across all public documentation
- Updated installation guide with native installer table and correct download URLs
- Updated user guide with current tab layout, command list, and directory structure

---

## [2.3.0] - 2026-02-21 — Official Beta Release

### Added
- **Numbered Error Codes**: All errors now display a unique `PLG-NNNN` identifier (e.g., `[PLG-1001] Request timed out`)
  - Category-based numbering for easy lookup: network (1xxx), validation (2xxx), filesystem (3xxx), source (4xxx), auth (5xxx), config (6xxx), workflow (7xxx), security (8xxx)
- **Native Installers**: Platform-specific installers now available for download
  - `.deb` package for Linux
  - `.exe` setup installer for Windows (includes PATH setup and uninstaller)
  - `.pkg` installer for macOS (arm64 + x64)
- **Developer Partnership Form**: Plugin developers can request integration with Pluginator through GitHub Issues

### Changed
- **Uninstall command**: User data at `~/.pluginator/` is now preserved by default — pass `--remove-data` to opt in to removal

### Fixed
- Resolved all CI/CD pipeline issues (lint, TypeScript, and test errors)
- Fixed theme marketplace loading and display
- Fixed health dashboard scrolling

---

## [2.2.5-2.2.8] - 2026-02-20

### Added
- **CLI `--no-ui` flag**: Skip interactive TUI for scripting and CI use
- **Stats view refresh**: Press `r` to refresh stats on the Stats tab
- **Health view controls**: Press `r` to refresh health, `Enter` to view plugin details
- **Recommendations legend**: Symbol legend bar showing type icons
- **Uninstall command**: `/uninstall` available in command palette
- **Command palette tier filtering**: Commands hidden if user lacks the required tier
- **Tab tier gating**: Stats, Health, Recommendations, and Performance tabs require Plus/Pro tier

### Changed
- BETA badge color changed to orange to match web app
- Home tab shortcuts removed — use command palette instead
- Server scan runs automatically after setup wizard completes

### Fixed
- First-run flow no longer asks user to edit JSON files — setup wizard handles it directly

## [2.2.4] - 2026-02-20

### Added
- **Registry API Integration**: Plugin registry loads from NinSys API with bundled fallback
- **Theme Gallery API**: Theme marketplace loads from live API with offline fallback
- **Uninstall support**: `pluginator uninstall` command with `--remove-data` option

### Changed
- All API calls hardened with timeouts and dynamic User-Agent headers
- Registry expanded to 101 curated plugins

## [2.2.0] - 2026-02-07

### Added
- **Auto-Source Assignment**: Scanned plugins automatically matched to the curated registry
  - First-run assigns all plugins; subsequent scans assign only new ones
  - Manual overrides are never overwritten
- **Plugin Source Editor**: View and edit plugin source configuration with `/plugin-edit` command
  - Press `e` or `Enter` in Plugins tab to open editor

## [2.1.0-2.1.3] - 2026-02-07

### Added
- **In-CLI Config Editor**: `/config-edit` command for editing configuration interactively
- **In-CLI Source Editor**: `/source-edit` command for editing custom sources
- **Setup Wizard Enhancements**: Path validation, cached paths, new steps for features and notifications
- **Beta Indicator**: BETA badge displayed in the header
- **Plugin Registry**: Expanded from 41 to 101 curated plugins

### Fixed
- Health dashboard items disappearing when scrolling
- Command palette always running top result instead of selected command
- Global key conflicts between Home tab and other tabs
- Server JAR default changed to test server (safer workflow)
- Various UI layout and navigation fixes

## [2.0.0-2.0.1] - 2026-02-03

### First Public Release

This release marks the first public version of Pluginator.

### Security
- SSRF prevention on all HTTP requests and panel URLs
- Command sanitization for server panel integrations
- Strict checksum verification for downloads (corrupted files automatically deleted)
- Dependency updates for known vulnerabilities

### Added
- **OAuth cancellation**: Press Escape to cancel OAuth login
- **GitHub rate limiting**: Proactive backoff when approaching API rate limits
- **Gradient progress bars**: Visual gradient pattern for all tiers

### Fixed
- Fixed broken NinSys API calls
- Force quit (Ctrl+C) now works during setup wizard
- Fixed 4 critical UI crashes during normal interaction
- Fixed virtual list rendering glitch across multiple views
- Backup now shows proper error for empty directories

## [1.71.0] - 2026-02-02

### Added
- **New Tabs**: Health (7), Recommendations (8), Performance (9) in main navigation
- **Plugin Health Dashboard**: Health scoring and issue detection
- **Smart Recommendations**: Plugin recommendations with filtering
- **Performance Dashboard**: Server performance analytics with charts
- **Conflict Detection Modal**: Detect and resolve plugin conflicts
- **New commands**: `/health`, `/recs`, `/perf`, `/conflicts`

## [1.70.0-1.70.6] - 2026-02-02

### Added
- **Server Performance Analytics**: Complete server monitoring system
  - TPS/MSPT sparkline graphs, memory and CPU visualization
  - Plugin performance rankings and optimization recommendations
  - Alert monitoring for low TPS, memory pressure, and high MSPT
  - Performance baselines for before/after comparison
- **Server Panel Integrations**: Connect to AMP, Pterodactyl, Crafty Controller, and MCSManager
  - Server status, metrics, console access, and power control

## [1.69.0-1.69.5] - 2026-02-02

### Added
- **Plugin Configuration Templates**: Create, share, and apply server configurations
  - Template engine with variables, conditionals, and loops
  - Create templates from existing config files with auto-detected variables
  - Preview and conflict detection before applying
  - Template library with favorites, collections, and history
  - Template marketplace for community sharing
  - Sync templates across multiple servers with drift detection

## [1.66.0-1.66.5] - 2026-02-02

### Added
- **Smart Plugin Recommendations**: Intelligent recommendation engine
  - Similar plugins, missing dependencies, popular, and trending suggestions
  - Plugin comparison tool with side-by-side stats and migration guides
  - Personalized discovery feed (New, Trending, Staff Picks, For You)
  - Community reviews and ratings aggregated from multiple sources
  - Save plugins to a wishlist for later

## [1.65.0] - 2026-02-02

### Added
- **Theme Marketplace**: Browse and install community themes with `/themes` command
  - Search, filter, and install themes from the marketplace
  - Max tier required for marketplace access

## [1.64.0] - 2026-02-02

### Added
- **Widget Dashboard**: Customizable dashboard with `/dashboard` command
  - Plugin summary, server status, updates, and quick actions widgets
  - Analytics, health, and live log streaming widgets
  - Layout editor for Plus tier users

## [1.61.0] - 2026-02-02

### Added
- **Data Management**: Export, import, preview, and backup scheduling
  - `/export` and `/import` commands for configuration sharing
  - `/preview` command for dry-run sync operations
  - `/backups` command for automatic backup scheduling

## [1.60.0] - 2026-02-02

### Added
- **Plugin Organization**: Tags, groups, and profiles
  - Press `t` to manage tags, `g` to manage groups
  - `/tags`, `/groups`, `/profiles` commands
  - Tier-gated: Tags (Free/Plus), Groups (Plus), Profiles (Pro)

## [1.59.0] - 2026-02-02

### Added
- **Favorites**: Mark plugins as favorites with `f` key, filter with `/favorites`
- **Stats View**: Plugin statistics via `/stats` command or tab 6
- **Accessibility Settings**: Sticky keys, slow keys, and visual feedback (Max tier)
- **Jump List Navigation**: Vim-style `Ctrl+O`/`Ctrl+I` to navigate view history

## [1.58.0-1.58.5] - 2026-02-01

### Added
- **Vim Mode**: Vim-style navigation and editing for premium users
  - Normal, insert, visual, and command modes
  - Forward/backward search with smart case
  - Marks system with jump list
- **Chord Keybindings**: Multi-key sequences with leader key (Space)
- **Macro Recording**: Record and playback keyboard macros with speed control
- **Custom Keybindings**: Configurable key bindings with presets (Default, Vim, Emacs, Minimal)
- **Quick Actions & Jump Lists**: Fuzzy finder and bookmark system
- **Accessibility Modes**: Sticky keys, slow keys, key repeat control, single-hand mode

## [1.55.0-1.55.3] - 2026-02-01

### Added
- **Premium ASCII Branding**: ASCII art banners and animated startup for paid tiers
- **View Transitions**: Smooth animated transitions between views (Pro/Max)
- **Premium Progress Bars**: Animated progress indicators with ETA display
- **Ambient Visual Effects**: Breathing animations, sparkles, and celebrations (Max tier)

## [1.54.0] - 2026-02-01

### Added
- **Plugin Registry Browser**: Browse the curated plugin catalog with `/registry` command
  - Search, category filtering, and plugin details
  - Download plugins directly from the registry
  - Quick filters: Popular (`p`), Verified (`v`), Installed (`n`)

### Fixed
- False "update available" for build number suffixes (e.g., `1.7.3-b131` is treated as `1.7.3`)
- Download failures now correctly show warning indicators
- Update results persist across app restarts

## [1.53.0] - 2026-01-31

### Added
- **Snapshot Management**: Selection dialog for restoring snapshots
  - Rename snapshots with `/snapshot:rename` command
  - Validation and corruption detection for snapshot archives

## [1.52.0-1.52.5] - 2026-01-30

### Added
- **Premium Gradient Text**: Animated gradient text for paid tier users across the UI
- **Schedule System**: Automated update execution with time windows
  - Background daemon with `/schedule:start`, `/schedule:stop`, `/schedule:status` commands
  - Auto-backup before updates with retention policy
  - Notifications via terminal, desktop, and webhooks (Discord/Slack)
  - Per-plugin schedule overrides and blackout dates

### Security
- SSRF validation on redirects, IPv6 blocking, web manifest URL validation

### Fixed
- Various data safety, accessibility, and type safety improvements

## [1.50.0] - 2026-01-29

### Added
- **Improved Plugin Data Management**: Better caching and preferences system
  - Plugin preferences persist between sessions
  - Scan results cached for faster startup

## [1.48.0] - 2026-01-26

### Added
- **Interactive Dependency Graph**: Visualize plugin dependencies with tree, matrix, and list views
  - Open with `/deps` command from the command palette
  - Export to DOT (Graphviz), Mermaid, JSON, and HTML formats
  - Cycle detection and missing dependency highlighting
- **Update Queue System**: Queue plugin updates for scheduled execution
  - Major version detection with optional skipping
  - Queue persistence and history tracking

## [1.41.0-1.41.4] - 2026-01-25

### Added
- **Conflict Detection**: Detect and resolve sync conflicts before syncing
  - Identify conflicts: newer files on target, different content, config files, size mismatches
  - Resolution options: skip, overwrite, rename, or backup-then-overwrite
- **Dry Run Mode**: Preview sync changes before executing
- **Backup Verification**: Checksum-based backup integrity verification
- **Atomic Sync**: Two-phase commit with staging, verification, and automatic rollback
- **Enhanced Caching**: Cache warming, preloading, and memory pressure monitoring

## [1.34.0-1.40.0] - 2026-01-25

### Added
- **Standardized Error Handling**: Typed errors with user-friendly messages (Network, Validation, Filesystem, Source, Auth, Config, Workflow, Security)
- **HTTP Circuit Breaker**: Prevents cascading network failures with per-host tracking
- **Per-File Sync Progress**: Granular phase and byte-level tracking during sync
- **Enhanced Schema Validation**: Path traversal protection, token validation, source-specific field validation
- **Plugin Rollback Logging**: Comprehensive logging for rollback operations

## [1.33.0-1.33.3] - 2026-01-24

### Added
- **Plugin Profiles**: Save and switch between server configurations
  - Create, import/export, and lock profiles
  - Apply modes: merge or replace
- **Session Recording**: Record and replay sessions with speed control
  - `Ctrl+R` to toggle recording, `Ctrl+P` to open recordings

## [1.30.0-1.32.0] - 2026-01-24

### Added
- **Dry Run / Preview Mode**: See what sync, migrate, update, backup, and restore operations will do before executing
- **Export/Import Configuration**: Backup and share configurations with merge/replace modes and checksum verification
- **Session History & Audit Log**: Track actions, query by type, and export to JSON/CSV

## [1.26.0-1.29.0] - 2026-01-24

### Added
- **Plugin Groups & Tags**: Organize plugins with custom tags (8 colors), built-in tags, and hierarchical groups
- **Favorites & Pinned**: Star plugins (`*` key) and pin to top (`p` key)
- **Quick Filters**: Filter by status, source, and category
- **Performance Optimization**: API caching, parallel operations, and automatic retry

## [1.22.0-1.25.0] - 2026-01-24

### Added
- **Plugin Statistics Dashboard**: Track update checks, downloads, errors, and server health scores
- **Plugin Rollback**: Restore previous versions with `/rollback` command
- **Plugin Conflict Detection**: Detect version mismatches, API incompatibilities, and duplicates
- **Plugin Dependency Resolution**: Dependency graphs, missing/circular dependency detection
- **Environment Modes**: Production, development, and test modes with `--env` flag
- **Update Scheduling**: Configurable intervals with quiet hours and `pluginator schedule` command
- **Sleep Prevention**: Prevents system sleep during long operations (macOS, Windows, Linux)

## [1.19.0] - 2026-01-24

### Added
- **Plugin Changelogs**: View plugin version history and changes
  - Fetch changelogs from Modrinth, GitHub, Hangar
  - Press `c` in Updates view to view changelog

## [1.12.0-1.18.0] - 2026-01-23

### Added
- **Auto-Updater**: `/update` and `/version` commands with install method detection
- **Install Script**: One-line installation for binary downloads
- **File Organization**: Bundled defaults separated from user files in `~/.pluginator/`
- **Migration Service**: Automatic migration from legacy file structure

### Security
- SSRF vulnerability fixes, localhost-only OAuth binding, session integrity verification

### Fixed
- Memory leaks: file descriptor cleanup, EventEmitter cleanup

## [1.6.0-1.11.0] - 2026-01-23

### Added
- **Hangar API Integration**: Full support for PaperMC's official plugin repository
- **Plugin Registry**: Curated database of 40+ popular plugins with pre-configured sources
- **Tiered Subscription System**: Free, Plus, Pro, and Max tiers with usage-based limits
- **Authentication**: Email/password, Google OAuth, and GitHub OAuth login
- **First-Launch Experience**: Welcome prompt for new users
- **New Commands**: `/login`, `/logout`, `/account`
- **Virtual List Scrolling**: Smooth scrolling with overscan buffer and log tailing
- **Encrypted Sessions**: Session tokens encrypted at rest with AES-256-GCM
- **Performance**: React.memo optimization across UI components

## [1.4.0-1.5.4] - 2026-01-16

### Added
- **Input System**: Robust keyboard handling with throttle/debounce and state machine
- **Jenkins CI Source**: Jenkins CI plugin source integration
- **Batch Download**: Download multiple plugins with progress tracking
- **Plugin Enable/Disable**: Toggle plugins on and off

### Fixed
- Jenkins nested folder paths and sources loading
- Download filename now reads actual plugin name from JAR
- Old plugin versions cleaned up after download

### Changed
- Downloads go to TEST server by default (safer workflow)

## [1.3.0-1.3.3] - 2026-01-16

### Added
- **Logging System**: Dual-format output with category filtering and performance timing
- **CLI `logs` command**: View and search application logs
- **Logs Tab**: New tab for viewing logs (shortcut: `5`)
- **Quick Backup**: `b` keyboard shortcut for instant backup

## [1.2.0-1.2.6] - 2026-01-14

### Added
- **Offline Detection**: Network connectivity indicator in status bar
- **Minecraft Version Fetching**: Dynamic version list from Mojang API

### Fixed
- Navigation bar duplication, setup wizard input issues, debug mode interference

### Changed
- Paper/Purpur sources upgraded to PaperMC v3 API

## [1.1.0] - 2026-01-14

### Added
- First-run detection with setup prompt
- Server JAR download (Paper and Purpur support)
- Command palette (triggered with `:`)
- Confirm dialog with keyboard navigation

### Fixed
- Keyboard input issues and help overlay soft lock

## [1.0.0] - 2026-01-11

Initial release of the TypeScript version.

### Core Features
- Plugin management for Minecraft servers
- Multi-source update checking (Spigot, Modrinth, GitHub, CurseForge, Web Manifest)
- Production/Test server workflow support
- Interactive terminal UI with customizable themes
- JAR file scanning and metadata extraction
- Checksum verification for downloads

---

*Pluginator was originally written in C++ (v2.0.3). This TypeScript rewrite started fresh at v1.0.0 with a completely new codebase.*
