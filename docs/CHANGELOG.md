# Changelog

All notable changes to Pluginator will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
