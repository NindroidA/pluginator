# Commands Reference

> **Last Updated:** March 04, 2026

Complete reference for Pluginator's command palette commands (~75 commands).

## Accessing Commands

Open the command palette with `/` or `:`, then type to search. Commands requiring a higher tier than your current subscription are hidden from the palette.

## Server Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Scan Servers | `/scan` | Scan plugin directories on both servers | Free |
| Sync Servers | `/sync` | Sync plugins from production to test server | Free |
| Migrate to Production | `/migrate` | Migrate tested plugins from test to production | Free |
| Download Server JAR | `/download` | Download latest Paper or Purpur server JAR | Free |

## Plugin Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Check for Updates | `/updates` | Check all plugins for available updates | Free |
| Rollback Plugin | `/rollback` | Restore a plugin to a previous version | Free |
| Plugin Conflicts | `/conflicts` | View and resolve plugin conflicts | Free |
| Plugin Discovery | `/discover` | Discover new plugins based on your setup | Free |
| Compare Plugins | `/compare` | Side-by-side plugin comparison | Free |
| Template Marketplace | `/templates` | Browse and apply plugin configuration templates | Free |
| Plugin Registry | `/registry` | Browse and install from the curated plugin registry | Free |
| Edit Plugin Source | `/pedit` | Edit a plugin's update source and settings | Free |
| Clear Filters | `/clearfilters` | Clear all active filters | Free |
| Manage Tags | `/tags` | Manage plugin tags | Plus |
| Manage Groups | `/groups` | Manage plugin groups | Plus |
| Plugin Profiles | `/profiles` | Save and load plugin configurations | Pro |
| Dependency Graph | `/deps` | View plugin dependency relationships | Pro |

## Navigation Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Go to Home | `/home` | Navigate to home dashboard | Free |
| Go to Servers | `/servers` | Navigate to servers view | Free |
| Go to Plugins | `/plugins` | Navigate to plugins list | Free |
| View Logs | `/logs` | View application debug logs | Free |
| Show Favorites | `/favorites` | Show only favorite plugins | Free |
| Universal Search | `/search` | Universal search across plugins, commands, and logs | Free |
| View Stats | `/stats` | View plugin statistics and health metrics | Plus |
| Health Dashboard | `/health` | View plugin health scores and issues | Pro |
| Recommendations | `/recs` | View plugin recommendations | Pro |
| Performance | `/perf` | View server performance analytics | Pro |
| Jump Back | `/jumpback` | Go to previous location (Ctrl+O) | Free |
| Jump Forward | `/jumpfwd` | Go to next location (Ctrl+I) | Free |

## Workflow Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Create Backup | `/backup` | Create a backup of plugin directories | Free |
| Setup Wizard | `/setup` | Configure Pluginator for your servers | Free |
| Refresh | `/refresh` | Reload configuration and rescan servers | Free |
| Clear Cache | `/clearcache` | Clear cached update results and version data | Free |
| Preview Sync | `/preview` | Preview what will happen during a sync (dry run) | Free |
| Validate JSON | `/validate` | Check all JSON configuration files for errors | Free |
| Reset Pluginator | `/reset` | Clear cache, logs, and session (preserves config) | Free |
| Backup Schedule | `/backups` | Configure automatic backup schedule and retention | Free |
| Update Queue | `/queue` | View and manage queued plugin updates | Free |
| Export Cache | `/exportcache` | Export cache contents for debugging | Free |
| Re-assign Sources | `/auto-source` | Reset all plugins to recommended sources from registry | Free |
| Update Schedule | `/schedule` | Configure automatic update checking schedule | Pro |
| Start Daemon | `/schedule:start` | Start the background update daemon | Free |
| Stop Daemon | `/schedule:stop` | Stop the background update daemon | Free |
| Daemon Status | `/schedule:status` | Show daemon status and execution history | Free |
| Record Macro | `/macro` | Start/stop recording a macro | Pro |
| Play Macro | `/play` | Play a recorded macro | Pro |
| Export Config | `/export` | Export plugins, servers, tags, and groups to file | Pro |
| Import Config | `/import` | Import plugins, servers, tags, and groups from file | Pro |

## Configuration Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Show Config | `/config` | View configuration settings | Free |
| Edit Config | `/config-edit` | Edit configuration settings interactively | Free |
| Manage Sources | `/sources` | Enable/disable and configure plugin sources | Free |

## Account Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Login | `/login` | Login to your Pluginator account | Free |
| Logout | `/logout` | Logout from your account | Free |
| Account | `/account` | View account and usage information | Free |
| Upgrade Plan | `/upgrade` | Upgrade your Pluginator subscription | Free |

## General Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Show Help | `/help` | Display keyboard shortcuts help | Free |
| Exit | `/exit` | Exit Pluginator | Free |
| Check for Updates | `/update` | Check for Pluginator updates and auto-install | Free |
| Show Version | `/version` | Show current version and install method | Free |
| Environment Info | `/env` | Show current environment and paths | Free |
| Cache Statistics | `/cachestats` | Show detailed cache statistics and performance | Free |
| Uninstall | `/uninstall` | Remove Pluginator and its data from this system | Free |

## Keyboard & Accessibility Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Keybindings | `/keys` | Customize keyboard shortcuts | Free |
| Chord Shortcuts | `/chords` | Show available chord key sequences | Free |
| Toggle Chords | `/chord` | Enable/disable chord key sequences | Plus |
| Quick Actions | `/quick` | Open quick actions palette (Ctrl+P) | Free |
| Toggle Vim Mode | `/vim` | Toggle vim-style navigation | Pro |
| Vim Help | `/vim-help` | Show vim mode reference | Free |
| Toggle Accessibility | `/a11y` | Toggle accessibility features | Max |
| Accessibility Settings | `/accessibility` | Configure accessibility features | Free |
| Macro Library | `/macros` | View and manage saved macros | Pro |

## Premium Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| Widget Dashboard | `/dash` | View customizable widget dashboard | Pro |
| Edit Dashboard | `/dash:edit` | Customize dashboard widgets and layout | Plus |
| Theme Marketplace | `/themes` | Browse and install community themes | Max |

## Debug Commands

| Command | Shortcut | Description | Notes |
|---------|----------|-------------|-------|
| Factory Reset | `/factoryreset` | Clear all data (DEBUG mode only) | Requires `PLUGINATOR_DEBUG=1` |

## Command Palette Tips

- Type any part of a command name or keyword to filter
- Shortcuts provide direct-match scoring for faster access
- Press `Enter` to execute the selected command
- Press `Esc` to close the palette
- Commands requiring a higher tier are automatically hidden
- `/quit` still fuzzy-matches to the Exit command

## Fuzzy Search Scoring

| Match Type | Score |
|------------|-------|
| Exact shortcut match | +200 |
| Shortcut starts with query | +100 |
| Name exact match | +80 |
| Name starts with query | +50 |
| Name contains query | +25 |
| Keyword match | +15 |
| Description match | +10 |

## Related

- [User Guide](USER_GUIDE.md) - General usage
- [Tier System](TIER_SYSTEM.md) - Subscription tiers and feature access
- [Keyboard Shortcuts](USER_GUIDE.md#keyboard-shortcuts) - All shortcuts
