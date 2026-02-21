# Changelog

All notable changes to Pluginator will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.48.0] - 2026-01-26

### Added
- **Interactive Dependency Graph**: Visualize plugin dependencies with tree, matrix, and list views
  - Open with `/deps` command from the command palette
  - Export to DOT (Graphviz), Mermaid, JSON, and HTML formats
  - Cycle detection and missing dependency highlighting
- **Update Queue System**: Queue plugin updates for scheduled execution
  - Major version detection with optional skipping
  - Queue persistence and history tracking

## [1.41.4] - 2026-01-25

### Added
- **Conflict Detection**: Detect and resolve sync conflicts before syncing
  - Identify conflicts: newer files on target, different content, config files, size mismatches
  - Resolution options: skip, overwrite, rename, or backup-then-overwrite
- **Test Scenario Runner**: Execute predefined test scenarios from dev CLI
  - Navigation and workflow test scenarios
  - Step-through debugging with assertions

## [1.41.3] - 2026-01-25

### Added
- **Dry Run Mode**: Preview sync changes before executing
  - Shows files to copy, update, delete, disable, enable, or skip
  - Displays total bytes to transfer and disk space required

## [1.41.2] - 2026-01-25

### Added
- **Backup Verification**: Checksum-based backup integrity verification
  - Generate manifests with file checksums
  - Verify backups against manifests
  - Quick integrity checks using total checksum

## [1.41.1] - 2026-01-25

### Added
- **Atomic Sync**: Two-phase commit pattern for robust sync operations
  - Stage files before committing
  - Verify staging integrity before applying
  - Automatic rollback on failure
- **Enhanced Caching**: Advanced caching with cache warming, preloading, and memory pressure monitoring
- **Session Playback**: Playback recorded dev sessions with speed control

## [1.41.0] - 2026-01-25

### Changed
- Improved test coverage across logging, version comparison, and update checking modules

## [1.40.0] - 2026-01-25

### Added
- **Consolidated App State**: Improved UI state management
- **HTTP Circuit Breaker**: Prevent cascading failures in network requests
  - Automatic blocking when too many failures occur
  - Per-host tracking for independent failure handling

## [1.39.0] - 2026-01-25

### Added
- **Source Cache Factory**: Unified caching interface for plugin sources

## [1.38.0] - 2026-01-25

### Added
- **Per-File Sync Progress**: Granular progress tracking during sync operations
  - Phase tracking: scanning, planning, backup, copying, verifying
  - Byte-level progress for large files

## [1.37.0] - 2026-01-25

### Added
- **Source Utilities**: Reusable utilities for plugin source implementations
  - Fluent HTTP request builder with auth support
  - Automatic retry with exponential backoff
  - Enhanced caching with statistics

## [1.36.0] - 2026-01-25

### Added
- **Enhanced Schema Validation**: Stricter validation for configuration files
  - Path traversal protection
  - GitHub token format validation
  - Source-type-specific required field validation
- **Rollback Logging**: Comprehensive logging for plugin rollback operations

## [1.35.0] - 2026-01-25

### Added
- **Primitive Hook System**: Reusable building blocks for async operations
  - Progress tracking with cancellation support
  - Unified throttle/debounce with configurable options
  - Stable callbacks that always use latest values

## [1.34.0] - 2026-01-25

### Added
- **Standardized Error Handling**: Comprehensive error types with user-friendly messages
  - Typed errors: Network, Validation, Filesystem, Source, Auth, Config, Workflow, Security
  - Error codes for programmatic handling
  - UI components for displaying errors

## [1.33.3] - 2026-01-24

### Added
- **Session Recording & Playback**: Record and replay dev sessions
  - Capture interactions, navigation, commands, and state changes
  - Speed control, pause/resume, step-through playback
  - Keyboard shortcuts: `Ctrl+R` to toggle recording, `Ctrl+P` to open recordings

## [1.33.2] - 2026-01-24

### Added
- **Debug Marker System**: Interactive debugging tools for development
  - Create markers with severity levels and status tracking
  - Add notes during debugging sessions
  - Export context for code review

## [1.33.1] - 2026-01-24

### Added
- **Developer Testing Tools**: Separate dev CLI for testing tier functionality
  - Press `1-4` to instantly switch between Free/Plus/Pro/Max tiers
  - No real API or account required for tier testing

## [1.33.0] - 2026-01-24

### Added
- **Plugin Profiles**: Save and switch between server configurations
  - Create profiles from current config or start empty
  - Apply modes: merge (add missing) or replace (full switch)
  - Import/export profiles as JSON for sharing
  - Color-coded profiles with lock protection

## [1.32.0] - 2026-01-24

### Added
- **Dry Run / Preview Mode**: See what operations will do before executing
  - Preview sync, migrate, update, backup, and restore operations
  - Change indicators: add (+), remove (-), modify (~), skip (○), conflict (!)
  - Space requirements and duration estimates

## [1.31.0] - 2026-01-24

### Added
- **Export/Import Configuration**: Backup and share Pluginator configurations
  - Export plugins, servers, favorites, tags, groups, settings
  - Import from file, URL, or text with merge/replace modes
  - Validation and compatibility checking
  - Checksum verification for data integrity

## [1.30.0] - 2026-01-24

### Added
- **Session History & Audit Log**: Comprehensive action tracking
  - Track downloads, syncs, backups, scans, config changes
  - Query by action type, plugin, or text search
  - Statistics: entry counts, session counts, success rates
  - Export to JSON and CSV

## [1.29.0] - 2026-01-24

### Added
- **Performance Optimization**: Caching, parallel execution, and monitoring
  - API response caching with TTL and LRU eviction
  - Parallel operations with configurable concurrency
  - Automatic retry with exponential backoff
  - Performance metrics tracking

## [1.28.0] - 2026-01-24

### Added
- **Plugin Groups & Tags**: Organize plugins with custom tags and groups
  - Create custom tags with 8 color options
  - Built-in tags: Essential, Optional, Experimental, Deprecated
  - Hierarchical groups with status tracking
  - Filter plugins by tags

## [1.27.0] - 2026-01-24

### Added
- **Favorites & Pinned**: Mark plugins as favorites and pin important ones
  - Favorites: starred plugins with optional notes (★)
  - Pinned: plugins always sorted to top (📌)
  - Toggle favorite with `*` key, toggle pin with `p` key

## [1.26.0] - 2026-01-24

### Added
- **Quick Filters**: Filter plugin lists by status, source, and category
  - Status filters: up-to-date, outdated, error, disabled
  - Source filters: Modrinth, Spigot, GitHub, Hangar, etc.
  - Category filters: admin, economy, permissions, world, etc.

## [1.25.0] - 2026-01-24

### Added
- **Environment Modes**: Production, development, and test environments
  - Automatic environment detection
  - Environment-specific paths and configurations
  - `--env` CLI flag and `PLUGINATOR_ENV` variable
  - `[DEV]` and `[TEST]` badges in status bar

## [1.24.0] - 2026-01-24

### Added
- **Update Scheduling**: Automated update checks with scheduler integration
  - Configurable intervals: hourly, daily, weekly, custom cron
  - Quiet hours support
  - `pluginator schedule` CLI command
  - Platform-specific scheduler command generation (cron, schtasks, launchd)

## [1.23.2] - 2026-01-24

### Added
- **Operation Flow Documentation**: Living documentation for codebase operations

## [1.23.1] - 2026-01-24

### Added
- **Sleep Prevention**: Prevent system sleep during long operations (sync, backup)
  - Cross-platform support: macOS, Windows, Linux

## [1.23.0] - 2026-01-24

### Added
- **Plugin Statistics Dashboard**: Comprehensive statistics tracking
  - Track update checks, downloads, errors per plugin
  - Source response times and check counts
  - Server health score with breakdown factors
  - Recommendations based on statistics

## [1.22.0] - 2026-01-24

### Added
- **Plugin Rollback**: Restore previous plugin versions
  - Track version history with hashes and metadata
  - Integrity verification before rollback
  - `/rollback` command in command palette

## [1.21.1] - 2026-01-24

### Fixed
- Security improvements: URL validation, YAML deserialization safety, SSRF protection, path traversal prevention
- Added error boundary for graceful error handling

## [1.21.0] - 2026-01-24

### Added
- **Plugin Conflict Detection**: Detect and warn about plugin conflicts
  - Version mismatches between servers
  - API incompatibilities
  - Duplicate plugins

## [1.20.0] - 2026-01-24

### Added
- **Plugin Dependency Resolution**: Automatic dependency checking
  - Build dependency graphs
  - Detect missing, circular, and optional dependencies
  - Visual dependency tree display

## [1.19.0] - 2026-01-24

### Added
- **Plugin Changelogs**: View plugin version history and changes
  - Fetch changelogs from Modrinth, GitHub, Hangar
  - Markdown rendering with syntax highlighting
  - Press `c` in Updates view to view changelog

## [1.18.0] - 2026-01-23

### Fixed
- Security improvements: SSRF vulnerability fix, localhost-only OAuth binding, session integrity verification
- Memory leak fixes: file descriptor cleanup, EventEmitter cleanup, unmount state update prevention

## [1.17.0] - 2026-01-23

### Added
- **Code Quality Audit**: Comprehensive multi-agent code review with security and performance improvements

## [1.16.0] - 2026-01-23

### Added
- **Development Infrastructure**: Planning and automation tools
  - Organized planning directory structure
  - Version bump automation

## [1.15.0] - 2026-01-23

### Changed
- Disabled custom themes (bundled presets only for consistency)
- Updated documentation

## [1.14.0] - 2026-01-23

### Added
- **GitHub & Release Management**: CI/CD workflows
  - Automated changelog transformation
  - Binary release workflow
  - Issue and discussion templates

## [1.13.0] - 2026-01-23

### Added
- **Plugin Developer Collaboration**: Partnership program infrastructure
  - Featured, Sponsored, and Premium partnership types
  - Developer outreach templates

## [1.12.0] - 2026-01-23

### Added
- **Auto-Updater System**: Self-update functionality
  - `/update` command to check for updates
  - `/version` command to display version and install method
  - Automatic update check on startup (cached 24 hours)
  - Install method detection (npm, bun, Homebrew, Scoop, apt, binary)
- **Install Script**: One-line installation for binary downloads
  - Detects OS and architecture automatically
  - Supports version selection and custom install paths

## [1.11.0] - 2026-01-23

### Added
- **File Organization**: Separated bundled and user files
  - Bundled defaults in package (read-only)
  - User files in `~/.pluginator/` (editable)
- **Sources Service**: Manage bundled and user-defined plugin sources
- **Migration Service**: Automatic migration from legacy file structure

## [1.10.4] - 2026-01-23

### Fixed
- List rendering issues where items briefly disappeared
- Progress indicator going backwards during update checks

## [1.10.0-1.10.3] - 2026-01-23

### Added
- **React.memo Optimization**: Performance improvements for UI components
  - Memoized display components to prevent unnecessary re-renders

## [1.9.0-1.9.4] - 2026-01-23

### Added
- **Virtual List**: Improved scrolling with overscan buffer
  - `pinToEnd` option for log tailing
  - Smoother scrolling experience

## [1.8.0-1.8.4] - 2026-01-23

### Added
- **Auth UI States**: Authentication flow states for login, account view, and first launch
- **Encrypted Session Storage**: Session tokens encrypted at rest with AES-256-GCM

## [1.7.0-1.7.3] - 2026-01-23

### Added
- **Crypto Infrastructure**: Token encryption at rest
  - Machine-specific identifier for key derivation
  - Secure buffer zeroing for key protection
- **Auth CLI Helpers**: Session management commands

## [1.6.0-1.6.8] - 2026-01-23

### Added
- **Hangar API Integration**: Full support for PaperMC's official plugin repository
- **Plugin Registry System**: Curated database of 40+ popular plugins with pre-configured sources
- **Tiered Subscription System**: Usage-based limits with Free, Plus, Pro, and Max tiers
- **Authentication System**: Email/password, Google OAuth, and GitHub OAuth login
- **First-Launch Experience**: Welcome prompt for new users
- **New Commands**: `/login`, `/logout`, `/account`
- **UI Components**: Tier badge, usage bars, upgrade prompts, login dialogs

### Changed
- Security and performance audits with various improvements

## [1.5.0-1.5.4] - 2026-01-17 - 2026-01-22

### Added
- Improved test coverage
- Reusable scroll components

### Fixed
- Jenkins nested folder paths
- Jenkins sources loading in UI
- Various download and sync improvements
- Download filename now reads actual plugin name from JAR
- Old plugin versions automatically cleaned up after download

### Changed
- Downloads now go to TEST server by default (safer workflow)
- Responsive version columns in Updates tab

## [1.4.0-1.4.1] - 2026-01-16

### Added
- **Input Sanity Check System**: Robust keyboard input handling
  - Throttle/debounce for expensive operations
  - UI state machine with distinct states
  - Input validation with debug logging
- Jenkins CI plugin source
- Batch download with progress tracking
- Plugin enable/disable functionality

## [1.3.0-1.3.3] - 2026-01-16

### Added
- **Enhanced Logging System**: Complete logging overhaul
  - Dual-format output: human-readable + JSONL file format
  - Category-based filtering
  - Performance timing
  - Operation tracing
- **CLI `logs` command**: View and search application logs
- **Logs UI Tab**: New tab for viewing logs (shortcut: `5`)
- `formatBytes()` utility for human-readable file sizes
- `b` keyboard shortcut for quick backup
- Input sanity check design document

## [1.2.0-1.2.6] - 2026-01-14 - 2026-01-16

### Added
- **Network Connectivity Detection**: Offline indicator in status bar
- Dynamic Minecraft version fetching from Mojang API
- Graceful offline fallback

### Fixed
- Nav bar duplication issues
- SetupWizard input and scrolling issues
- Debug mode interference with UI
- Various rendering glitches

### Changed
- Paper/Purpur sources use PaperMC v3 API

## [1.1.0] - 2026-01-14

### Added
- First-run detection with setup prompt
- Compact UI layout for smaller terminals
- Server JAR download (Paper and Purpur support)
- Command palette (triggered with `:`)
- Version number in header
- Confirm dialog with keyboard navigation
- Alternate screen buffer for clean rendering

### Fixed
- Various keyboard input issues
- Help overlay soft lock
- Duplicate rendering on tab switch

## [1.0.0] - 2026-01-11

Initial release of the TypeScript version.

### Core Features
- Plugin management for Minecraft servers
- Multi-source update checking (Spigot, Modrinth, GitHub, CurseForge, Web Manifest)
- Production/Test server workflow support
- Interactive terminal UI
- Customizable themes
- JAR file scanning and metadata extraction
- Checksum verification for downloads

---

## Previous C++ Version

The original C++ implementation reached version 2.0.3. This TypeScript rewrite starts fresh at v1.0.0 with a new codebase while maintaining feature parity.

For the legacy C++ changelog, see the [pluginator-deprecated](https://github.com/NindroidA/pluginator-deprecated) repository.
