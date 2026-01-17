# Pluginator

[![Built with Bun](https://img.shields.io/badge/Built%20with-Bun-f9f1e1?logo=bun&logoColor=f9f1e1&labelColor=14151a)](https://bun.sh)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.0-3178c6?logo=typescript&logoColor=white)](https://www.typescriptlang.org/)
[![React](https://img.shields.io/badge/Ink-React%20CLI-61dafb?logo=react&logoColor=white)](https://github.com/vadimdemedes/ink)
[![License](https://img.shields.io/badge/License-Proprietary-red)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)]()

> Minecraft server plugin manager with multi-source update checking and downloading

Pluginator is a powerful CLI tool for managing Minecraft server plugins. It supports multiple plugin sources, production/test server synchronization, and automated backups.

## Features

- **Multi-Source Updates** - Check updates from Spigot, Modrinth, GitHub, CurseForge, Jenkins
- **Server Sync** - Synchronize plugins between production and test servers
- **Backup System** - Automated backups with rotation
- **Interactive UI** - Modern terminal interface with keyboard navigation
- **Cross-Platform** - Works on Windows, macOS, and Linux

## Installation

### Download Binary

Download the latest release for your platform:

| Platform | Download |
|----------|----------|
| Windows (x64) | [pluginator-windows-x64.zip](https://github.com/NindroidA/pluginator/releases/latest) |
| macOS (Intel) | [pluginator-darwin-x64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| macOS (Apple Silicon) | [pluginator-darwin-arm64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| Linux (x64) | [pluginator-linux-x64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |
| Linux (ARM64) | [pluginator-linux-arm64.tar.gz](https://github.com/NindroidA/pluginator/releases/latest) |

### Quick Install

**macOS/Linux:**
```bash
# Download and extract (example for macOS ARM64)
curl -L https://github.com/NindroidA/pluginator/releases/latest/download/pluginator-darwin-arm64.tar.gz | tar xz
chmod +x pluginator-*
sudo mv pluginator-* /usr/local/bin/pluginator
```

**Windows (PowerShell):**
```powershell
# Download and extract
Invoke-WebRequest -Uri "https://github.com/NindroidA/pluginator/releases/latest/download/pluginator-windows-x64.zip" -OutFile "pluginator.zip"
Expand-Archive -Path "pluginator.zip" -DestinationPath "."
# Add to PATH or move to desired location
```

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
```

## Documentation

- [User Guide](docs/USER_GUIDE.md) - Complete usage documentation
- [Changelog](docs/CHANGELOG.md) - Version history and release notes
- [Security](docs/SECURITY.md) - Security policy and reporting

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `1-5` | Switch tabs |
| `:` or `/` | Open command palette |
| `s` | Scan servers |
| `u` | Check updates |
| `b` | Create backup |
| `?` | Show help |
| `Ctrl+C` (2x) | Quit |

## Support

- **Bug Reports:** [Open an issue](https://github.com/NindroidA/pluginator/issues/new?template=bug_report.md)
- **Feature Requests:** [Open an issue](https://github.com/NindroidA/pluginator/issues/new?template=feature_request.md)
- **Discussions:** [GitHub Discussions](https://github.com/NindroidA/pluginator/discussions)

## License

Copyright (c) 2026 Andrew Curtis (NindroidA). All Rights Reserved.

This is proprietary software. See [LICENSE](LICENSE) for details.
