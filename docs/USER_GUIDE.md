# Pluginator User Guide

> **Last Updated:** April 30, 2026

Complete guide to using Pluginator for Minecraft server plugin management.

## Table of Contents

- [Getting Started](#getting-started)
- [Interactive Mode](#interactive-mode)
- [Subscription Tiers](#subscription-tiers)
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
- [Plugin Notes & Config Sync](#plugin-notes--config-sync)
- [Theming](#theming)
- [Post-Action Hooks](#post-action-hooks)
- [Troubleshooting](#troubleshooting)

## Getting Started

### Prerequisites

Install Pluginator using your preferred method. See [INSTALLATION.md](INSTALLATION.md) for all options.

```bash
# macOS
brew tap NindroidA/pluginator && brew install pluginator

# Windows
scoop bucket add pluginator https://github.com/NindroidA/scoop-pluginator && scoop install pluginator

# npm (any platform)
npm install -g @nindroidsystems/pluginator
```

### Initial Setup

Run the setup wizard:

```bash
pluginator setup
```

This will guide you through:

1. Setting your production server path (with path validation)
2. Setting your test server path (with path validation)
3. Selecting Minecraft version and server type
4. Configuring features, panel connections, and notifications
5. Scanning for existing plugins
6. Auto-assigning update sources from the curated registry

**Auto-Source Assignment:** After scanning, Pluginator automatically matches your plugins to the curated registry (104 plugins) and assigns optimal update sources. Manually configured sources are never overwritten. Re-run via `/auto-source` at any time.

**Path Validation:** When you enter a server path, Pluginator verifies the directory exists and contains plugin JAR files. If validation fails, you can:

- **Retry** — Re-enter the path (your previous input is pre-filled)
- **Continue Anyway** — Proceed without plugins (useful for new server setups)

**Cached Paths:** Re-running the setup wizard pre-fills paths from your current config, so you don't have to retype them.

### User Data Directory

Pluginator stores its data at `~/.pluginator/`:

```
~/.pluginator/
├── config.json             # User configuration
├── custom-registry.json    # User plugin registry entries
├── custom-sources.json     # User source overrides
├── session.json            # Auth session (encrypted)
├── notifications.json      # Notification history (v2.9.1)
├── .salt                   # Encryption salt
├── servers/                # Per-server plugin data
│   └── <server-id>/
│       └── plugins.json
├── backups/                # Backup archives
├── cache/                  # Cache files
└── logs/                   # Application logs
```

## Interactive Mode

Launch the interactive terminal UI:

```bash
pluginator
```

The UI was overhauled in the v2.7.x series with a unified design system:

- **Bordered Panel components** wrap every major section with embedded titles and footers
- **EnhancedTable** drives the Plugins / Updates / Servers / Health views with auto-sized columns and responsive column hiding
- **KeyHintsBar** at the bottom of every view shows context-relevant shortcuts
- **EmptyState** components replace blank views with centered icon/title/description/action layouts
- **Scrollbar** appears on long lists with a track (`│`) and thumb (`┃`)
- **Breadcrumb** shows your navigation trail (Tab > Name > Detail)

### Tabs

The UI has 9 tabs:

| # | Tab | Description | Tier |
|---|-----|-------------|------|
| 1 | **Home** | Dashboard with Server Status, Updates, and Quick Actions cards | Free |
| 2 | **Servers** | Side-by-side comparison of production vs test plugins | Plus |
| 3 | **Plugins** | Full plugin list with search, filtering, and detail panel | Free |
| 4 | **Updates** | Available updates with progress + changelog view | Free |
| 5 | **Logs** | Application logs with filtering and auto-scroll | Plus |
| 6 | **Stats** | Plugin statistics, source breakdowns, version distribution | Plus |
| 7 | **Health** | Plugin health scores, security advisories, compatibility | Pro |
| 8 | **Recs** | Smart plugin recommendations | Pro |
| 9 | **Perf** | Server performance analytics with panel integrations | Pro |

Press `1`–`9` to jump to a tab. Tabs requiring a higher tier display a lock icon and strikethrough; pressing the shortcut shows an upgrade notification.

The visible tabs in the header can be customized via `/tabs` (Tab Visibility).

#### Home Dashboard

The Home view is now a true dashboard (v2.7.12):

- **Server Status card** — prod/test paths, plugin counts, last scan time
- **Updates card** — total available, last check time, error count
- **Quick Actions grid** — Scan / Sync / Check Updates / Backup buttons
- On terminals ≥120 columns wide, Server Status and Updates are shown side-by-side
- A startup tips panel rotates 20 tier-aware tips below the cards (v2.9.0)

#### Logs Tab

The Logs tab provides real-time access to application logs (Plus tier).

**Keyboard Shortcuts in Logs:**

- `f` — Filter by log level (All / Info / Warn / Error / Debug)
- `a` — Toggle auto-scroll (automatically scroll to newest logs)
- `r` — Refresh logs
- `Ctrl+D` — Quick access from any tab

**Features:**

- Color-coded log levels for easy identification
- Duration indicators for slow operations (>100ms shown in yellow, >1000ms in red)
- Structured context display for detailed operation information
- Daily log files with automatic cleanup

**Log Levels:**

- **Info** (blue) — Normal operation events
- **Warn** (yellow) — Non-critical issues
- **Error** (red) — Failures and exceptions
- **Debug** (gray) — Detailed diagnostic information (requires `PLUGINATOR_DEBUG=1`)

### Keyboard Shortcuts

Pluginator uses a state-aware input system that validates keys against the current UI state.

#### Global Keys (Available in Idle State)

| Key | Action | Notes |
|-----|--------|-------|
| `Tab` | Next tab | |
| `Shift+Tab` | Previous tab | |
| `1`–`9` | Jump to tab | Locked tabs show upgrade prompt |
| `j` / `↓` | Move down | |
| `k` / `↑` | Move up | |
| `g` | Jump to top | |
| `G` | Jump to bottom | |
| `Space` | Toggle selection | |
| `Enter` | Confirm action | |
| `Esc` | Cancel/close dialog | Context-dependent |
| `/` or `:` | Open command palette | |
| `Ctrl+P` | Open quick actions palette | |
| `?` | Show help overlay | |
| `Ctrl+C` | Quit (press twice) | 2s timeout |
| `Ctrl+D` | Jump to Logs tab | Plus tier |
| `Ctrl+O` | Jump back (vim-style) | Plus tier |
| `Ctrl+I` | Jump forward (vim-style) | Plus tier |

#### State-Specific Keys

Keys may be disabled in certain UI states:

**Progress State** (operation running):

- Only `Esc` (if cancellable) and `Ctrl+C` work
- All other keys are ignored to prevent accidental actions

**Command Palette** (when `/` or `:` is pressed):

- `↑` / `↓` — Navigate commands
- `Enter` — Execute command
- `Esc` / `Ctrl+C` — Close palette
- Any printable character — Search

**Dialogs** (confirmations, selections):

- Navigation and confirmation keys only
- Other actions disabled until dialog is closed

**Help/Config Overlays:**

- Any key closes the overlay
- Returns to previous state

### Command Palette

Press `/` or `:` to open the command palette. Type to search commands.

Common commands:

| Command | Description |
|---------|-------------|
| `/scan` | Scan plugin directories on both servers |
| `/sync` | Sync plugins from production to test |
| `/updates` | Check all plugins for available updates |
| `/backup` | Create a backup of plugin directories |
| `/setup` | Run the setup wizard |
| `/config-edit` | Edit configuration interactively |
| `/themes` | Browse and install community themes (Max tier) |
| `/login` | Login to your Pluginator account |
| `/account` | View account, tier, and today's usage |
| `/notifications` | View notification history |
| `/plugin-edit` | Edit a plugin's update source and settings |
| `/schedule` | Configure automatic update checking (Pro tier) |
| `/history` | View operation history |
| `/reset` | Clear cache, logs, and session (preserves config) |
| `/selfupdate` | Check for Pluginator updates and auto-install |
| `/help` | Display keyboard shortcuts |
| `/exit` | Exit Pluginator |

See [COMMANDS.md](COMMANDS.md) for the full list of ~75 commands.

**Command Palette Navigation:**

- `↑` / `↓` — Navigate through commands
- `Enter` — Execute selected command
- `Esc` — Close palette
- Commands requiring a higher tier are hidden automatically

### Quick Actions Palette

Press `Ctrl+P` to open the quick actions palette — a streamlined fuzzy finder over your most-used actions, plugins, and bookmarks. Use it when you know roughly what you want but don't remember the exact command name.

## Subscription Tiers

Pluginator uses a tiered subscription system. The free tier is intentionally narrow — enough to try the workflow, but daily plugin management really starts at Plus.

| Tier | Price | Update Checks/Day | Downloads/Day | Syncs/Day | Migrations/Day |
|------|-------|-------------------|---------------|-----------|----------------|
| **Free** | $0 | 1 | 5 | 1 | 0 |
| **Plus** | $9.99 (one-time) | 10 | 50 | 5 | 3 |
| **Pro** | $2.99/mo or $24.99/yr | 25 | 150 | 15 | 10 |
| **Max** | $4.99/mo or $44.99/yr | Unlimited | Unlimited | Unlimited | Unlimited |

### Feature Access

| Feature | Free | Plus | Pro | Max |
|---------|------|------|-----|-----|
| Scan, Sync, Backup, Updates | Yes | Yes | Yes | Yes |
| Home / Plugins / Updates tabs | Yes | Yes | Yes | Yes |
| Servers tab (prod vs test compare) | — | Yes | Yes | Yes |
| Logs tab | — | Yes | Yes | Yes |
| Stats Dashboard | — | Yes | Yes | Yes |
| Tags / Groups / Dashboard widgets | — | Yes | Yes | Yes |
| Jump-list (`Ctrl+O` / `Ctrl+I`), chord shortcuts | — | Yes | Yes | Yes |
| Migrations (test → prod) | — | Yes | Yes | Yes |
| Health Dashboard | — | — | Yes | Yes |
| Recommendations | — | — | Yes | Yes |
| Performance Analytics | — | — | Yes | Yes |
| Profiles / Macros / Vim mode / Schedule | — | — | Yes | Yes |
| Export / Import | — | — | Yes | Yes |
| Theme Marketplace | — | — | — | Yes |
| Accessibility toggle | — | — | — | Yes |

Plus tier holders receive a permanent discount on Pro/Max subscriptions. See [TIER_SYSTEM.md](TIER_SYSTEM.md) for full details.

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

- `-t, --type <type>` — Filter by source type (spigot, modrinth, github, hangar, etc.)
- `--all` — Include disabled plugins

### Sync Plugins

Copy plugins from production to test server:

```bash
pluginator sync
```

Options:

- `-f, --force` — Overwrite existing files
- `--dry-run` — Preview changes without copying

### Migrate Plugins

Move plugins from test to production:

```bash
pluginator migrate
```

Options:

- `--no-backup` — Skip automatic backup
- `--dry-run` — Preview changes

### Create or Restore Backup

```bash
# Create a backup
pluginator backup

# Restore from backup (use the -r flag)
pluginator backup -r ~/.pluginator/backups/backup-prod-2026-04-30T10-30-00.tar.gz
```

Options:

- `-o, --output <path>` — Custom backup location
- `-t, --type <type>` — Backup type (prod, test, both)
- `-r, --restore <file>` — Restore the specified backup archive

### View Logs

View and search application logs:

```bash
pluginator logs
```

Options:

- `--level <level>` — Filter by level (info, warn, error, debug)
- `--grep <pattern>` — Search for pattern in messages
- `--since <date>` — Show logs since date (YYYY-MM-DD)
- `--follow` — Real-time tail (like `tail -f`)
- `--json` — Output raw JSONL format

Examples:

```bash
# View only errors
pluginator logs --level error

# Search for "sync" in logs
pluginator logs --grep sync

# Follow logs in real-time
pluginator logs --follow

# Get machine-readable JSON output
pluginator logs --json --since 2026-04-25
```

### Other Commands

```bash
# Run setup wizard
pluginator setup

# Auto-update all plugins (scan + check + download)
pluginator auto-update

# Login / logout (or run /login in the UI)
pluginator login
pluginator logout

# Self-update Pluginator
pluginator self-update

# Show help / version
pluginator --help
pluginator --version

# Use specific theme
pluginator --theme ocean
```

## Plugin Configuration

Plugins are configured in `~/.pluginator/servers/<server-id>/plugins.json`. Use `/plugin-edit` (or `/pedit`) to edit interactively, or edit the file directly.

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

### Supported Sources

Pluginator supports 8 plugin sources:

| Source | `type` | Identifier field | Notes |
|--------|--------|------------------|-------|
| Spigot | `spigot` | `resourceId` | via Spiget API |
| Modrinth | `modrinth` | `projectSlug` | Generous rate limits |
| GitHub | `github` | `repoSlug` | Owner/repo format |
| Hangar | `hangar` | `projectSlug` | PaperMC's official catalog |
| GeyserMC | `geysermc` | `projectName` | Bedrock support |
| Bukkit / CurseForge | `curseforge` | `projectId` | API key required |
| Jenkins | `jenkins` | `jobUrl` | Custom CI builds |
| Web Manifest | `web` | `manifestUrl` | Self-hosted plugins |

#### Spigot Plugins

Find the resource ID in the Spigot URL:

`https://www.spigotmc.org/resources/essentialsx.9089/` → `9089`

```json
{
  "name": "EssentialsX",
  "type": "spigot",
  "resourceId": "9089"
}
```

#### Modrinth Plugins

Use the project slug from the URL:

`https://modrinth.com/plugin/luckperms` → `luckperms`

```json
{
  "name": "LuckPerms",
  "type": "modrinth",
  "projectSlug": "luckperms"
}
```

#### GitHub Plugins

Use owner/repo format:

```json
{
  "name": "Vault",
  "type": "github",
  "repoSlug": "MilkBowl/Vault"
}
```

#### Hangar Plugins

Use the project slug from `https://hangar.papermc.io/<author>/<slug>`:

```json
{
  "name": "Maintenance",
  "type": "hangar",
  "projectSlug": "Maintenance"
}
```

### Optional Fields

```json
{
  "name": "TestPlugin",
  "type": "spigot",
  "resourceId": "12345",
  "version": "1.0.0",            // Current installed version
  "enabled": true,                // Enable/disable (default: true)
  "disableOnTest": true,          // Disable when synced to test
  "mcVersion": "1.21.1",          // Target MC version
  "filenamePattern": ".*\\.jar",  // Regex for filename matching
  "notes": "Custom permissions"   // Free-form notes (v2.9.0)
}
```

## Server Configuration

Configuration is stored at `~/.pluginator/config.json`. Edit it interactively with `/config-edit`, or via the `/setup` wizard.

Key settings include:

- **Server paths** — Production and test plugin directories
- **Minecraft version** — Target version for compatibility checks
- **Backup settings** — Retention count and backup directory
- **API keys** — Optional GitHub token, CurseForge API key
- **Post-action hooks** — Shell commands to run after sync/download/backup/etc. (v2.9.0)

### Environment Variables

```bash
# Enable debug logging
PLUGINATOR_DEBUG=1 pluginator

# Custom API endpoint
PLUGINATOR_API_URL=https://api.nindroidsystems.com

# Disable colors
NO_COLOR=1 pluginator
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
| Hangar | Generous | PaperMC's official API |
| CurseForge | Varies | Requires API key |

## Sync Workflow

Sync copies plugins from production to test server.

### Process

1. Scans production plugins directory
2. Compares with test server plugins
3. Identifies new or updated plugins
4. Copies files to test server
5. Handles `disableOnTest` plugins by adding `.jardis` suffix

### Use Cases

- Testing updates before deploying to production
- Setting up a new test server
- Keeping test server in sync with production

### Preview Sync

Use `/preview` (or `pluginator sync --dry-run`) to see exactly what a sync would do — no files are touched.

### Config File Sync (v2.9.0)

The Servers tab can also discover and sync plugin YAML/JSON config files between servers (not just JARs). Use the `/config-sync` workflow to see which plugin configs differ between prod and test.

## Migrate Workflow

Migrate moves tested plugins from test to production. Requires Plus tier.

### Process

1. Creates automatic backup of production
2. Scans test server plugins
3. Validates changes (compatibility checks against your MC version)
4. Copies files to production
5. Cleans up old versions if needed

### Safety Features

- Automatic backup before migration
- Dry-run mode for preview
- Confirmation prompts
- Rollback instructions in logs (and via `/rollback`)

## Backup System

### Creating Backups

Backups are tar.gz archives containing:

- All plugin JAR files
- plugins.json configuration
- Timestamp in filename

Schedule automatic backups via `/backups` (Backup Schedule).

### Automatic Cleanup

Old backups are automatically removed based on the `MAX_BACKUPS` setting in `config.json`.

### Backup Naming

Format: `backup-{type}-{timestamp}.tar.gz`

Example: `backup-prod-2026-04-30T10-30-00.tar.gz`

### Restoring from Backup

```bash
# Use the CLI (preferred)
pluginator backup -r ~/.pluginator/backups/backup-prod-2026-04-30T10-30-00.tar.gz

# Or extract manually
tar -xzf backup-prod-2026-04-30T10-30-00.tar.gz -C /path/to/restore
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

### Theme Marketplace

*Requires Max tier*

Browse and install community themes:

```bash
# Open the theme marketplace
/themes
```

The marketplace loads themes from the NinSys API. If offline, a bundled gallery of curated themes is available as a fallback. Themes can be installed by URL or browsed by category (dark / light / accessibility).

## Health Dashboard

*Requires Pro tier*

The Health tab provides per-plugin health monitoring.

### Health Scores

Each plugin receives a health score (0-100) based on:

- **Compatibility** (40%) — Minecraft/server version support
- **Security** (30%) — Known vulnerabilities, update recency
- **Update Status** (30%) — Available updates, version age

### Health Grades

| Grade | Score | Meaning |
|-------|-------|---------|
| A | 90-100 | Excellent |
| B | 75-89 | Good |
| C | 60-74 | Fair |
| D | 40-59 | Poor |
| F | 0-39 | Critical |

### Commands

- `/health` — Open health dashboard
- `/conflicts` — View plugin conflicts
- `/deps` — View the dependency graph

## Performance Analytics

*Requires Pro tier*

The Performance tab monitors server performance.

### Metrics

- TPS (Ticks Per Second)
- MSPT (Milliseconds Per Tick)
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

## Recommendations

*Requires Pro tier*

The Recommendations tab suggests plugins based on:

- Plugins commonly used together
- Popular plugins you don't have
- Similar alternatives to your plugins
- Upgrade suggestions

Use `/recs` to open. Dismissed and saved recommendations persist between sessions.

## Templates

Apply pre-configured plugin bundles via `/templates`.

### Built-in Templates

- **Essential Starter** — LuckPerms, Vault, EssentialsX
- **Economy Bundle** — Vault + economy plugins
- **World Protection** — WorldGuard, CoreProtect
- **Admin Tools** — moderation, anti-cheat, logging

## Organization

Organize plugins with:

- **Tags** — Custom labels for filtering (`/tags`, Plus tier)
- **Groups** — Collections for bulk actions (`/groups`, Plus tier)
- **Profiles** — Saved configurations (`/profiles`, Pro tier)
- **Favorites** — Quick access to frequent plugins (`/favorites`)

## Plugin Notes & Config Sync

### Plugin Notes (v2.9.0)

Add free-form notes to any plugin via `/plugin-edit`. Notes appear in the plugin detail panel and are stored in your local preferences (not on the plugin's JAR). Useful for tracking custom permissions, server-specific tweaks, or "do not auto-update" reminders.

### Config File Sync (v2.9.0)

Beyond JAR sync, Pluginator can discover and sync plugin YAML/JSON config files between servers. Use it from the Servers tab to keep config drift in check between prod and test.

### Notification History (v2.9.1)

All toast notifications are persisted to `~/.pluginator/notifications.json`. Open `/notifications` to browse past notifications, see when each operation completed, and review error toasts you may have missed.

## Post-Action Hooks

*Added in v2.9.1*

Configure shell commands to run after specific operations:

- `post-sync` — After successful sync
- `post-download` — After plugin download
- `post-backup` — After backup creation
- `post-update-check` — After update check completes
- `post-migrate` — After migration to production

Configure via `/config-edit` under the `hooks` section. Hooks run with the operation's environment context as variables and are useful for restarting the server, sending Discord webhooks, or syncing with external tooling.

## Security

As of v2.8.0, Pluginator hardens its HTTP layer:

- **SSRF blocklist** — RFC 1918 / loopback / link-local addresses are refused for outbound requests, preventing a malicious source manifest from probing your internal network
- **Atomic file writes** — All config writes use a temp-file + rename pattern so a crash mid-write can't corrupt your config
- **Token guidance** — `/config-edit` flags tokens stored in plain config and recommends environment variables instead

## Troubleshooting

### Common Issues

**"No plugins found"**

- Check your `plugins.json` file exists
- Verify the JSON syntax is valid (`/validate`)
- Ensure plugins have correct type and identifiers

**"Failed to check updates"**

- Check your internet connection
- Verify API keys if using GitHub or CurseForge
- Check if the source API is accessible (try `/auto-source` to retry)

**"Permission denied"**

- Ensure you have write access to server directories
- Check file ownership
- Run with appropriate permissions

**"Version comparison failed"**

- Plugin may have non-standard version format
- Set explicit `version` in plugin config

**"Locked tab" notification**

- Servers / Logs / Stats require Plus, Health / Recs / Perf require Pro, Theme Marketplace requires Max
- See [TIER_SYSTEM.md](TIER_SYSTEM.md) for full feature access

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
- Input validation (accepted/rejected inputs with reasons)
- State transitions (UI state changes with triggers)
- Throttled operations (rate-limited action attempts)

### Reset Options

| Command | Effect |
|---------|--------|
| `/clearcache` | Clears the API/update cache only |
| `/reset` | Clears cache, logs, and session — preserves config |
| `/factoryreset` | Wipes everything (debug mode only) |

### Getting Help

1. Check the [Troubleshooting Guide](TROUBLESHOOTING.md)
2. Join the [NindroidSystems Discord](https://discord.gg/nkwMUaVSYH) for quick community help
3. Search [existing issues](https://github.com/NindroidA/pluginator/issues)
4. Open a new issue with debug logs

### Log Files

Logs are stored in `~/.pluginator/logs/`:

```
~/.pluginator/logs/
├── pluginator-2026-04-30.log
├── pluginator-2026-04-29.log
└── ...
```

Log files are created daily and automatically cleaned up.
