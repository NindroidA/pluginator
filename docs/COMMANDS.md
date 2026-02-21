# Commands Reference

> **Last Updated:** February 21, 2026

Complete reference for Pluginator's command palette commands (74 commands).

## Accessing Commands

Open the command palette with `/` or `:`, then type to search. Commands requiring a higher tier than your current subscription are hidden from the palette.

## Server Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/scan` | `scan` | Scan plugin directories on both servers | Free |
| `/sync` | `sync` | Sync plugins from production to test server | Free |
| `/migrate` | `migrate` | Migrate tested plugins from test to production | Free |
| `/download` | `download` | Download latest Paper or Purpur server JAR | Free |

## Plugin Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/updates` | `updates` | Check all plugins for available updates | Free |
| `/rollback` | `rollback` | Restore a plugin to a previous version | Free |
| `/conflicts` | `conflicts` | View and resolve plugin conflicts | Free |
| `/discover` | `discover` | Discover new plugins based on your setup | Free |
| `/compare` | `compare` | Side-by-side plugin comparison | Free |
| `/templates` | `templates` | Browse and apply plugin configuration templates | Free |
| `/registry` | `registry` | Browse and install from the curated plugin registry | Free |
| `/pedit` | `pedit` | Edit a plugin's update source and settings | Free |
| `/clearfilters` | `clearfilters` | Clear all active filters | Free |
| `/tags` | `tags` | Manage plugin tags | Plus |
| `/groups` | `groups` | Manage plugin groups | Plus |
| `/profiles` | `profiles` | Save and load plugin configurations | Pro |
| `/deps` | `deps` | View plugin dependency relationships | Pro |

## Navigation Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/home` | `home` | Navigate to home dashboard | Free |
| `/servers` | `servers` | Navigate to servers view | Free |
| `/plugins` | `plugins` | Navigate to plugins list | Free |
| `/logs` | `logs` | View application debug logs | Free |
| `/favorites` | `favorites` | Show only favorite plugins | Free |
| `/search` | `search` | Universal search across plugins, commands, and logs | Free |
| `/stats` | `stats` | View plugin statistics and health metrics | Plus |
| `/health` | `health` | View plugin health scores and issues | Pro |
| `/recs` | `recs` | View plugin recommendations | Pro |
| `/perf` | `perf` | View server performance analytics | Pro |
| `/jumpback` | `jumpback` | Go to previous location (Ctrl+O) | Free |
| `/jumpfwd` | `jumpfwd` | Go to next location (Ctrl+I) | Free |

## Workflow Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/backup` | `backup` | Create a backup of plugin directories | Free |
| `/setup` | `setup` | Configure Pluginator for your servers | Free |
| `/refresh` | `refresh` | Reload configuration and rescan servers | Free |
| `/clearcache` | `clearcache` | Clear cached update results and version data | Free |
| `/preview` | `preview` | Preview what will happen during a sync (dry run) | Free |
| `/validate` | `validate` | Check all JSON configuration files for errors | Free |
| `/reset` | `reset` | Clear cache, logs, and session (preserves config) | Free |
| `/backups` | `backups` | Configure automatic backup schedule and retention | Free |
| `/queue` | `queue` | View and manage queued plugin updates | Free |
| `/exportcache` | `exportcache` | Export cache contents for debugging | Free |
| `/schedule` | `schedule` | Configure automatic update checking schedule | Pro |
| `/schedule:start` | `schedule:start` | Start the background update daemon | Free |
| `/schedule:stop` | `schedule:stop` | Stop the background update daemon | Free |
| `/schedule:status` | `schedule:status` | Show daemon status and execution history | Free |
| `/macro` | `macro` | Start/stop recording a macro | Pro |
| `/play` | `play` | Play a recorded macro | Pro |
| `/export` | `export` | Export plugins, servers, tags, and groups to file | Pro |
| `/import` | `import` | Import plugins, servers, tags, and groups from file | Pro |

## Configuration Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/config` | `config` | View configuration settings | Free |
| `/config-edit` | `config-edit` | Edit configuration settings interactively | Free |
| `/sources` | `sources` | Enable/disable and configure plugin sources | Free |

## Account Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/login` | `login` | Login to your Pluginator account | Free |
| `/logout` | `logout` | Logout from your account | Free |
| `/account` | `account` | View account and usage information | Free |
| `/upgrade` | `upgrade` | Upgrade your Pluginator subscription | Free |

## General Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/help` | `help` | Display keyboard shortcuts help | Free |
| `/quit` | `quit` | Exit Pluginator | Free |
| `/update` | `update` | Check for Pluginator updates | Free |
| `/version` | `version` | Show current version and install method | Free |
| `/env` | `env` | Show current environment and paths | Free |
| `/cachestats` | `cachestats` | Show detailed cache statistics and performance | Free |
| `/uninstall` | `uninstall` | Remove Pluginator and its data from this system | Free |

## Keyboard & Accessibility Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/keys` | `keys` | Customize keyboard shortcuts | Free |
| `/chords` | `chords` | Show available chord key sequences | Free |
| `/chord` | `chord` | Enable/disable chord key sequences | Free |
| `/quick` | `quick` | Open quick actions palette (Ctrl+P) | Free |
| `/vim` | `vim` | Toggle vim-style navigation | Free |
| `/vim-help` | `vim-help` | Show vim mode reference | Free |
| `/a11y` | `a11y` | Toggle accessibility features | Free |
| `/accessibility` | `accessibility` | Configure accessibility features | Free |
| `/macros` | `macros` | View and manage saved macros | Free |

## Premium Commands

| Command | Shortcut | Description | Tier |
|---------|----------|-------------|------|
| `/dash` | `dash` | View customizable widget dashboard | Pro |
| `/dash:edit` | `dash:edit` | Customize dashboard widgets and layout | Free |
| `/themes` | `themes` | Browse and install community themes | Max |

## Debug Commands

| Command | Shortcut | Description | Notes |
|---------|----------|-------------|-------|
| `/factoryreset` | `factoryreset` | Clear all data (DEBUG mode only) | Requires `PLUGINATOR_DEBUG=1` |

## Command Palette Tips

- Type any part of a command name or keyword to filter
- Shortcuts provide direct-match scoring for faster access
- Press `Enter` to execute the selected command
- Press `Esc` to close the palette
- Commands requiring a higher tier are automatically hidden

## Fuzzy Search Scoring

The palette uses a scoring system to rank results:

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
