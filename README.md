# Pluginator

<a href="https://bun.sh" target="_blank" rel="noopener noreferrer"><img src="https://img.shields.io/badge/Built%20with-Bun-f9f1e1?logo=bun&logoColor=f9f1e1&labelColor=14151a" alt="Built with Bun"></a>
<a href="https://www.typescriptlang.org/" target="_blank" rel="noopener noreferrer"><img src="https://img.shields.io/badge/TypeScript-5.0-3178c6?logo=typescript&logoColor=white" alt="TypeScript"></a>
<a href="https://github.com/vadimdemedes/ink" target="_blank" rel="noopener noreferrer"><img src="https://img.shields.io/badge/Ink-React%20CLI-61dafb?logo=react&logoColor=white" alt="Ink React CLI"></a>
<a href="LICENSE"><img src="https://img.shields.io/badge/License-Proprietary-red" alt="License"></a>
<img src="https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-blue" alt="Platform">

A powerful Minecraft server plugin manager with multi-source update checking, sync/backup workflows, and a modern terminal UI.

## Features

### Core
- **Multi-Source Updates** — Check updates from Spigot, Modrinth, GitHub, Hangar, Bukkit, Jenkins, GeyserMC, and custom web manifests (8 source types)
- **Server Sync** — Synchronize plugins between production and test servers
- **Smart Backups** — Timestamped tar.gz backups with automatic cleanup, atomic writes, and crash recovery
- **Modern Terminal UI** — Interactive, keyboard-driven interface with 9 tabs and customizable themes
- **Plugin Registry** — 120+ popular plugins pre-configured with verified sources
- **Auto-Updater** — Automatic update checking with one-command updates
- **Checksum Verification** — SHA-256 verification for all downloads

### Analytics & Health
- **Health Dashboard** — 5-component health scoring (compatibility, security, version, maintenance, dependencies) with A+/F grading
- **Performance Analytics** — Server performance monitoring with metrics collection and alerts
- **Recommendations Engine** — Smart plugin recommendations based on your setup
- **Server Panel Integrations** — Connect to AMP, Pterodactyl, Crafty Controller, and MCSManager

### Organization & Workflows
- **Tags & Groups** — Organize plugins with custom tags and groups
- **Profiles** — Save and switch between different plugin configurations
- **Scheduled Updates** — Configure automatic update windows with major version handling
- **Data Import/Export** — Export and import plugin configurations
- **Widget Dashboard** — Customizable dashboard with health, analytics, and streaming widgets
- **Plugin Notes** — Attach notes to individual plugins
- **Notification History** — Persistent toast log accessible via `/notifications`
- **Post-Action Hooks** — Run user-configured shell commands after sync/download/backup/migrate

### Power-User
- **Command Palette** — 80+ commands accessible via `:` or `/` with fuzzy search
- **Vim Mode** — Optional modal navigation (Plus+) with motions, marks, search, and macros
- **Jump List** — `Ctrl+o` / `Ctrl+i` to navigate between recent positions
- **Quick Actions** — `Ctrl+P` for context-aware quick actions
- **Custom Keybindings** — 5 presets (default, vim, emacs, minimal, custom) with full inheritance

### Security
- **SSRF Protection** — Blocks private-IP and metadata-endpoint URLs by default
- **Atomic File Writes** — Crash-safe writes for cache, config, and journal files
- **Operation Journal** — Records destructive operations with crash recovery on next launch
- **PID Locks** — File-based exclusive locks prevent concurrent operations

## Installation

### Download Binary

| Platform | Download |
|----------|----------|
| Windows (x64) | [pluginator-windows-x64.zip](https://github.com/NindroidA/pluginator/releases/latest) |
| macOS (Intel) | [pluginator-darwin-x64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| macOS (Apple Silicon) | [pluginator-darwin-arm64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| Linux (x64) | [pluginator-linux-x64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| Linux (ARM64) | [pluginator-linux-arm64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |

### Quick Install

```bash
# macOS (Homebrew)
brew tap NindroidA/pluginator && brew install pluginator

# Windows (Scoop)
scoop bucket add pluginator https://github.com/NindroidA/scoop-pluginator && scoop install pluginator

# npm (any platform)
npm install -g @nindroidsystems/pluginator

# Bun (any platform)
bun install -g @nindroidsystems/pluginator

# curl | bash (Linux/macOS)
curl -fsSL https://raw.githubusercontent.com/NindroidA/pluginator/main/scripts/install.sh | bash
```

See [Installation Guide](docs/INSTALLATION.md) for detailed instructions.

## Quick Start

```bash
# Initialize configuration
pluginator setup

# Launch interactive UI
pluginator

# Or use CLI commands
pluginator scan --prod          # Scan production server
pluginator check-updates        # Check for plugin updates
pluginator sync                 # Sync prod to test
pluginator self-update          # Update Pluginator itself
```

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `1-9` | Switch tabs (Home/Servers/Plugins/Updates/Logs/Stats/Health/Recs/Perf) |
| `:` or `/` | Open command palette (80+ commands) |
| `Ctrl+P` | Quick actions menu |
| `Ctrl+O` / `Ctrl+I` | Jump back/forward (history) |
| `j` / `k` or `↑` / `↓` | Navigate lists |
| `g` / `G` | Jump to top/bottom |
| `?` | Show help |
| `Ctrl+C` (2x) | Quit |

## Tier System

| Tier | Price | Update Checks/Day | Downloads/Day | Tabs |
|------|-------|---|---|---|
| Free | $0 | 1 | 5 | Home/Plugins/Updates only |
| Plus | $9.99 (one-time) | 50 | 25 | + Servers/Logs/Stats |
| Pro | $2.99/mo or $24.99/yr | 100 | 50 | + Health/Recs/Perf, scheduling, vim mode |
| Max | $4.99/mo or $44.99/yr | Unlimited | Unlimited | All features, priority support |

Plus owners get discounted upgrades ($1.99/mo Pro, $3.99/mo Max). See [Tier System](docs/TIER_SYSTEM.md) for full details.

## Documentation

- [Installation](docs/INSTALLATION.md) — Quick install for all platforms
- [User Guide](docs/USER_GUIDE.md) — Complete usage documentation
- [Commands Reference](docs/COMMANDS.md) — Full command palette reference
- [Plugin Registry](docs/PLUGIN_REGISTRY.md) — Curated plugin database
- [Tier System](docs/TIER_SYSTEM.md) — Subscription tiers and features
- [Troubleshooting](docs/TROUBLESHOOTING.md) — Common issues and fixes
- [Error Codes](docs/ERROR_CODES.md) — Error code reference (PLG-NNNN)
- [Security](docs/SECURITY.md) — Security policy and reporting
- [Changelog](docs/CHANGELOG.md) — Version history

## Support

- **Bug Reports:** [Open an issue](https://github.com/NindroidA/pluginator/issues/new?template=bug-report.yml)
- **Feature Requests:** [Open an issue](https://github.com/NindroidA/pluginator/issues/new?template=feature-request.yml)
- **General Feedback:** [Open an issue](https://github.com/NindroidA/pluginator/issues/new?template=general-feedback.yml)
- **Plugin Partnerships:** [Partnership request](https://github.com/NindroidA/pluginator/issues/new?template=plugin-developer-request.yml)

## Development

This project's development has been accelerated through the use of AI-assisted development tools, including GitHub Copilot, Claude Code, and other AI programming assistants. These tools have enhanced productivity while maintaining code quality and best practices. All AI-generated code has been reviewed, tested, and refined to ensure reliability.

## License

Copyright (c) 2026 Nindroid Systems (Andrew Curtis - NindroidA). All Rights Reserved.

This is proprietary software. See [LICENSE](LICENSE) for details.
