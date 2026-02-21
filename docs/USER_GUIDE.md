# Pluginator User Guide

> **Last Updated:** February 21, 2026

Complete guide to using Pluginator for Minecraft server plugin management.

## Table of Contents

- [Getting Started](#getting-started)
- [Interactive Mode](#interactive-mode)
- [Command Line Usage](#command-line-usage)
- [Plugin Configuration](#plugin-configuration)
- [Server Configuration](#server-configuration)
- [Update Checking](#update-checking)
- [Sync Workflow](#sync-workflow)
- [Migrate Workflow](#migrate-workflow)
- [Backup System](#backup-system)
- [Health Dashboard](#health-dashboard)
- [Performance Analytics](#performance-analytics)
- [Recommendations](#recommendations)
- [Templates](#templates)
- [Organization](#organization)
- [Theming](#theming)
- [Troubleshooting](#troubleshooting)

## Getting Started

### Prerequisites

1. Install [Bun](https://bun.sh):
   ```bash
   curl -fsSL https://bun.sh/install | bash
   ```

2. Install Pluginator:
   ```bash
   bun install -g pluginator
   ```

### Initial Setup

Run the setup wizard:

```bash
pluginator setup
```

This will guide you through:
1. Setting production server path (with path validation)
2. Setting test server path (with path validation)
3. Selecting Minecraft version and server type
4. Configuring features, panel connections, and notifications
5. Scanning for existing plugins
6. Auto-assigning update sources from the curated registry

**Auto-Source Assignment (v2.2.0):** After scanning, Pluginator automatically matches your plugins to the curated registry and assigns optimal update sources. Manually configured sources are never overwritten.

**Path Validation (v2.1.2):** When you enter a server path, Pluginator verifies the directory exists and contains plugin JAR files. If validation fails, you can:
- **Retry** - Re-enter the path (your previous input is pre-filled)
- **Continue Anyway** - Proceed without plugins (useful for new server setups)

**Cached Paths (v2.1.2):** Re-running the setup wizard pre-fills paths from your current config, so you don't have to retype them.

### Directory Structure

After setup, your structure should look like:

```
pluginator/
├── config/
│   └── pluginator.config    # Main configuration
├── data/
│   ├── backups/             # Backup archives
│   ├── logs/                # Application logs
│   └── plugin_versions.json # Version cache
├── plugins.json             # Plugin configuration
└── SERVER/                  # (or your configured paths)
    ├── prod/plugins/        # Production plugins
    └── test/plugins/        # Test server plugins
```

## Interactive Mode

Launch the interactive terminal UI:

```bash
pluginator
```

### Navigation

The UI has 9 tabs (v2.0):

1. **Home** - Dashboard with quick actions and status
2. **Servers** - Side-by-side comparison of production vs test
3. **Plugins** - Full plugin list with search and filtering
4. **Updates** - Available updates with changelog view
5. **Logs** - Application logs with filtering
6. **Sync** - Sync status and history
7. **Health** - Plugin health scores and issues (v2.0)
8. **Recommendations** - Smart plugin recommendations (v2.0)
9. **Performance** - Server performance analytics (v2.0)

Press `1-9` to jump directly to any tab.

#### Logs Tab (v1.3.0)

The Logs tab provides real-time access to application logs with the following features:

**Keyboard Shortcuts in Logs:**
- `f` - Filter by log level (All/Info/Warn/Error/Debug)
- `a` - Toggle auto-scroll (automatically scroll to newest logs)
- `r` - Refresh logs
- `Ctrl+D` - Quick access from any tab

**Features:**
- Color-coded log levels for easy identification
- Duration indicators for slow operations (>100ms shown in yellow, >1000ms in red)
- Structured context display for detailed operation information
- Daily log files with automatic cleanup

**Log Levels:**
- **Info** (blue) - Normal operation events
- **Warn** (yellow) - Non-critical issues
- **Error** (red) - Failures and exceptions
- **Debug** (gray) - Detailed diagnostic information (requires `PLUGINATOR_DEBUG=1`)

**Reading Logs:**

Each log entry shows:
- Timestamp
- Level indicator
- Message
- Context (structured data about the operation)
- Duration (if operation took significant time)

### Keyboard Shortcuts

Pluginator uses a state-aware input system (v1.4.0) that validates keys against the current UI state.

#### Global Keys (Available in Idle State)

| Key | Action | Notes |
|-----|--------|-------|
| `Tab` | Next tab | |
| `Shift+Tab` | Previous tab | |
| `1-9` | Jump to tab (Home/Servers/Plugins/Updates/Logs/Sync/Health/Recs/Perf) | |
| `j` / `↓` | Move down | |
| `k` / `↑` | Move up | |
| `g` | Jump to top | |
| `G` | Jump to bottom | |
| `Space` | Toggle selection | |
| `Enter` | Confirm action | |
| `Esc` | Cancel/close dialog | Context-dependent |
| `/` or `:` | Open command palette | |
| `?` | Show help overlay | |
| `Ctrl+C` | Quit (press twice) | 2s timeout |
| `Ctrl+D` | Jump to Logs tab | Quick access |
| `r` | Refresh current view | Home tab only |
| `s` | Scan servers | Home tab only, throttled 500ms |
| `u` | Check for updates | Home tab only, throttled 1000ms |
| `b` | Create backup | Home tab only |

#### State-Specific Keys

Keys may be disabled in certain UI states:

**Progress State** (operation running):
- Only `Esc` (if cancellable) and `Ctrl+C` work
- All other keys are ignored to prevent accidental actions

**Command Palette** (when `/` or `:` is pressed):
- `↑` / `↓` - Navigate commands
- `Enter` - Execute command
- `Esc` / `Ctrl+C` - Close palette
- Any printable character - Search

**Dialogs** (confirmations, selections):
- Navigation and confirmation keys only
- Other actions disabled until dialog is closed

**Help/Config Overlays**:
- Any key closes the overlay
- Returns to previous state

### Command Palette

Press `/` or `:` to open the command palette. Type to search commands:

| Command | Description |
|---------|-------------|
| `/scan` | Scan plugin directories on both servers |
| `/sync` | Sync plugins from production to test |
| `/updates` | Check all plugins for available updates |
| `/backup` | Create a backup of plugin directories |
| `/refresh` | Reload configuration and rescan servers |
| `/download` | Download latest Paper or Purpur server JAR |
| `/config` | View configuration settings |
| `/home` | Navigate to home dashboard |
| `/servers` | Navigate to servers view |
| `/plugins` | Navigate to plugins list |
| `/help` | Display keyboard shortcuts |
| `/quit` | Exit Pluginator |

Command Palette Navigation:
- `↑`/`↓` - Navigate through commands
- `Enter` - Execute selected command
- `Esc` - Close palette

## Subscription Tiers

Pluginator uses a tiered subscription system. Some features require a higher tier:

| Feature | Free | Plus | Pro | Max |
|---------|------|------|-----|-----|
| Scan, Sync, Backup, Updates | Yes | Yes | Yes | Yes |
| Stats Dashboard | - | Yes | Yes | Yes |
| Health Dashboard | - | - | Yes | Yes |
| Recommendations | - | - | Yes | Yes |
| Performance Analytics | - | - | Yes | Yes |
| Theme Marketplace | - | - | - | Yes |

Locked tabs show a lock icon in the header. Attempting to access a locked tab shows an upgrade notification. Commands requiring a higher tier are hidden from the command palette.

## Command Line Usage

### CLI-Only Mode

Use `--no-ui` to skip the interactive TUI:

```bash
pluginator --no-ui
```

This lists available CLI subcommands, useful for scripting and CI environments.

### Check for Updates

```bash
pluginator check-updates
```

Options:
- `-t, --type <type>` - Filter by source type (spigot, modrinth, github)
- `--all` - Include disabled plugins

### Sync Plugins

Copy plugins from production to test server:

```bash
pluginator sync
```

Options:
- `-f, --force` - Overwrite existing files
- `--dry-run` - Preview changes without copying

### Migrate Plugins

Move plugins from test to production:

```bash
pluginator migrate
```

Options:
- `--no-backup` - Skip automatic backup
- `--dry-run` - Preview changes

### Create Backup

```bash
pluginator backup
```

Options:
- `-o, --output <path>` - Custom backup location
- `-t, --type <type>` - Backup type (prod, test, both)

### View Logs

View and search application logs (v1.3.0):

```bash
pluginator logs
```

Options:
- `--level <level>` - Filter by level (info, warn, error, debug)
- `--grep <pattern>` - Search for pattern in messages
- `--since <date>` - Show logs since date (YYYY-MM-DD)
- `--follow` - Real-time tail (like `tail -f`)
- `--json` - Output raw JSONL format

Examples:

```bash
# View only errors
pluginator logs --level error

# Search for "sync" in logs
pluginator logs --grep sync

# View today's logs
pluginator logs --since 2025-01-16

# Follow logs in real-time
pluginator logs --follow

# Get machine-readable JSON output
pluginator logs --json --since 2025-01-16
```

### Other Commands

```bash
# Run setup wizard
pluginator setup

# Show help
pluginator --help

# Show version
pluginator --version

# Use specific theme
pluginator --theme ocean
```

## Plugin Configuration

Plugins are configured in `plugins.json`:

```json
{
  "plugins": [
    {
      "name": "EssentialsX",
      "type": "spigot",
      "resourceId": "9089",
      "enabled": true
    }
  ]
}
```

### Spigot Plugins

Find the resource ID in the Spigot URL:
`https://www.spigotmc.org/resources/essentialsx.9089/`
                                              ^^^^

```json
{
  "name": "EssentialsX",
  "type": "spigot",
  "resourceId": "9089"
}
```

### Modrinth Plugins

Use the project slug from the URL:
`https://modrinth.com/plugin/luckperms`
                             ^^^^^^^^^

```json
{
  "name": "LuckPerms",
  "type": "modrinth",
  "projectSlug": "luckperms"
}
```

### GitHub Plugins

Use owner/repo format:

```json
{
  "name": "Vault",
  "type": "github",
  "repoSlug": "MilkBowl/Vault"
}
```

### Optional Fields

```json
{
  "name": "TestPlugin",
  "type": "spigot",
  "resourceId": "12345",
  "version": "1.0.0",           // Current installed version
  "enabled": true,               // Enable/disable (default: true)
  "disableOnTest": true,         // Disable when synced to test
  "mcVersion": "1.21.1",         // Target MC version
  "filenamePattern": ".*\\.jar"  // Regex for filename matching
}
```

## Server Configuration

Main configuration file: `config/pluginator.config`

```ini
# Debug mode (0 = off, 1 = on)
PLUGINATOR_DEBUG=0

# Server paths (absolute or relative)
PROD_SERVER_PATH=./SERVER/prod/plugins
TEST_SERVER_PATH=./SERVER/test/plugins

# Backup settings
BACKUP_DIR=./data/backups
MAX_BACKUPS=5

# Logging
LOGS_DIR=./data/logs
MAX_LOG_DAYS=30

# Target Minecraft version
MINECRAFT_VERSION=1.21.1

# API settings
API_TIMEOUT=30
DOWNLOAD_THREADS=2

# Optional API keys
GITHUB_TOKEN=ghp_xxxxxxxxxxxx
CURSEFORGE_API_KEY=xxxxxxxxxx
```

### Environment Variables

All config values can be overridden with environment variables:

```bash
PROD_SERVER_PATH=/custom/path pluginator
```

## Update Checking

### How It Works

1. Pluginator reads your `plugins.json`
2. For each enabled plugin, queries the appropriate API
3. Compares installed version with latest available
4. Reports which plugins have updates

### Version Comparison

Pluginator handles various version formats:
- Semantic: `1.0.0`, `2.1.3`
- Prefixed: `v1.0.0`
- Build numbers: `1.0.0-b123`
- Snapshots: `1.0.0-SNAPSHOT`

### Rate Limiting

Each source has rate limits:

| Source | Limit | Notes |
|--------|-------|-------|
| Spigot | ~100/min | Via Spiget API |
| Modrinth | 300/min | Generous limits |
| GitHub | 60/hr | 5000/hr with token |
| CurseForge | Varies | Requires API key |

## Sync Workflow

Sync copies plugins from production to test server.

### Process

1. Scans production plugins directory
2. Compares with test server plugins
3. Identifies new or updated plugins
4. Copies files to test server
5. Handles `disableOnTest` plugins by adding `.disabled` suffix

### Use Cases

- Testing updates before deploying to production
- Setting up a new test server
- Keeping test server in sync with production

## Migrate Workflow

Migrate moves tested plugins from test to production.

### Process

1. Creates automatic backup of production
2. Scans test server plugins
3. Validates changes
4. Copies files to production
5. Cleans up old versions if needed

### Safety Features

- Automatic backup before migration
- Dry-run mode for preview
- Confirmation prompts
- Rollback instructions in logs

## Backup System

### Creating Backups

Backups are tar.gz archives containing:
- All plugin JAR files
- plugins.json configuration
- Timestamp in filename

### Automatic Cleanup

Old backups are automatically removed based on `MAX_BACKUPS` setting.

### Backup Naming

Format: `backup-{type}-{timestamp}.tar.gz`

Example: `backup-prod-2025-01-15T10-30-00.tar.gz`

### Restoring from Backup

```bash
# Extract backup
tar -xzf backup-prod-2025-01-15T10-30-00.tar.gz -C /path/to/restore

# Copy to server
cp -r /path/to/restore/* /your/server/plugins/
```

## Theming

Pluginator includes four built-in color themes:

| Theme | Description |
|-------|-------------|
| `default` | Purple/violet tones |
| `ocean` | Blue/cyan tones |
| `forest` | Green/emerald tones |
| `sunset` | Orange/amber tones |

### Selecting a Theme

```bash
# Via command line
pluginator --theme ocean

# Via config file (~/.pluginator/config.json)
{
  "theme": "ocean"
}
```

Themes are bundled presets designed for optimal readability. Custom themes are not supported to ensure consistent UI behavior across updates.

### Theme Gallery

Browse and install community themes from the theme gallery:

```bash
# Open the theme gallery in interactive mode
# Use the command palette: /themes
```

The gallery loads themes from the NinSys API. If offline, a bundled gallery of 22 curated themes is available as a fallback.

## Health Dashboard

*Added in v2.0*

The Health Dashboard (Tab 7) provides plugin health monitoring:

### Health Scores

Each plugin receives a health score (0-100) based on:
- **Compatibility** (40%) - Minecraft/server version support
- **Security** (30%) - Known vulnerabilities, update recency
- **Update Status** (30%) - Available updates, version age

### Health Grades

| Grade | Score | Meaning |
|-------|-------|---------|
| A | 90-100 | Excellent |
| B | 75-89 | Good |
| C | 60-74 | Fair |
| D | 40-59 | Poor |
| F | 0-39 | Critical |

### Commands

- `/health` - Open health dashboard
- `/conflicts` - View plugin conflicts

See [Health System](systems/HEALTH_SYSTEM.md) for full details.

## Performance Analytics

*Added in v2.0*

The Performance Dashboard (Tab 9) monitors server performance:

### Metrics

- TPS (Ticks Per Second)
- Memory usage
- Player count
- Plugin load times

### Features

- Real-time metrics display
- Historical charts
- Configurable alerts
- Baseline detection

### Panel Integrations

Connect to server management panels:
- AMP
- Pterodactyl
- Crafty Controller
- MCSManager

See [Analytics](architecture/ANALYTICS.md) for full details.

## Recommendations

*Added in v2.0*

The Recommendations tab (Tab 8) suggests plugins based on:

- Plugins commonly used together
- Popular plugins you don't have
- Similar alternatives to your plugins
- Upgrade suggestions

### Commands

- `/recs` - Open recommendations

## Templates

*Added in v2.0*

Apply pre-configured plugin bundles:

```bash
/templates
```

### Built-in Templates

- **Survival Server** - EssentialsX, LuckPerms, Vault, WorldGuard
- **Creative Server** - WorldEdit, FAWE, PlotSquared
- **Minigames** - Citizens, HolographicDisplays, PlaceholderAPI
- **Network Hub** - DeluxeHub, ServerSelectorX, TAB

See [Templates](systems/TEMPLATES.md) for full details.

## Organization

*Added in v2.0*

Organize plugins with:

- **Tags** - Custom labels for filtering (`/tags`)
- **Groups** - Collections for bulk actions (`/groups`)
- **Profiles** - Saved configurations (`/profiles`)
- **Favorites** - Quick access to frequent plugins (`/favorites`)

See [Organization](systems/ORGANIZATION.md) for full details.

## Troubleshooting

### Common Issues

**"No plugins found"**
- Check your `plugins.json` file exists
- Verify the JSON syntax is valid
- Ensure plugins have correct type and identifiers

**"Failed to check updates"**
- Check your internet connection
- Verify API keys if using GitHub or CurseForge
- Check if the source API is accessible

**"Permission denied"**
- Ensure you have write access to server directories
- Check file ownership
- Run with appropriate permissions

**"Version comparison failed"**
- Plugin may have non-standard version format
- Set explicit `version` in plugin config

### Debug Mode

Enable debug logging:

```bash
# Via environment variable
PLUGINATOR_DEBUG=1 pluginator

# Or in config file
PLUGINATOR_DEBUG=1
```

Debug logs include:
- API requests and responses
- Version comparison details
- File operations
- **Input validation** (v1.4.0) - Accepted/rejected inputs with reasons
- **State transitions** (v1.4.0) - UI state changes with triggers
- **Throttled operations** (v1.4.0) - Rate-limited action attempts

**Input Validation Logging:**

When debug mode is enabled, all keyboard inputs are validated against the current UI state and logged:

```json
{
  "level": "debug",
  "message": "Input rejected: 's' in progress state",
  "timestamp": "2025-01-16T10:30:45.123Z",
  "context": {
    "key": "s",
    "state": "progress",
    "valid": false,
    "reason": "operation in progress"
  }
}
```

**State Transition Logging:**

UI state changes are tracked with triggers:

```json
{
  "level": "info",
  "message": "UI state transition: idle → command-palette",
  "timestamp": "2025-01-16T10:30:45.456Z",
  "context": {
    "from": "idle",
    "to": "command-palette",
    "trigger": "/ or :"
  }
}
```

This helps diagnose:
- Why certain keys aren't working (wrong state)
- Unexpected state transitions
- Race conditions with async operations

### Getting Help

1. Check the [FAQ](https://github.com/NindroidA/pluginator/wiki/FAQ)
2. Join the [NindroidSystems Discord](https://discord.gg/nkwMUaVSYH) for quick community help
3. Search [existing issues](https://github.com/NindroidA/pluginator/issues)
4. Open a new issue with debug logs

### Log Files

Logs are stored in the configured `LOGS_DIR`:

```
data/logs/
├── pluginator-2025-01-15.log
├── pluginator-2025-01-14.log
└── ...
```

Old logs are automatically cleaned up based on `MAX_LOG_DAYS`.
