# Installation Guide

> **Last Updated:** February 21, 2026

## Quick Install

### macOS (Homebrew)

```bash
brew tap nindroida/pluginator
brew install pluginator
```

### Windows (Scoop)

```bash
scoop bucket add pluginator https://github.com/NindroidA/scoop-pluginator
scoop install pluginator
```

### Linux (Debian/Ubuntu .deb)

```bash
# Download the .deb from the latest release
curl -LO https://github.com/NindroidA/pluginator/releases/latest/download/pluginator-linux-x64.deb

# Install
sudo dpkg -i pluginator-linux-x64.deb
```

### Any Platform (npm)

```bash
npm install -g pluginator
```

### Any Platform (Bun)

```bash
bun install -g pluginator
```

### One-Line Script

```bash
curl -fsSL https://raw.githubusercontent.com/NindroidA/pluginator/main/scripts/install.sh | bash
```

The script automatically detects your OS and architecture.

## Downloads

All downloads are available at:
https://github.com/NindroidA/pluginator/releases/latest

### Native Installers

| Platform | File | Notes |
|----------|------|-------|
| macOS (Apple Silicon) | `.pkg` | Double-click to install to `/usr/local/bin/` |
| macOS (Intel) | `.pkg` | Double-click to install to `/usr/local/bin/` |
| Windows | `-setup.exe` | Installer with PATH setup and uninstaller |
| Linux (Debian/Ubuntu) | `.deb` | Install with `sudo dpkg -i` |

### Standalone Binaries

| Platform | Architecture | Filename |
|----------|--------------|----------|
| macOS | Apple Silicon (M1/M2/M3) | `pluginator-{version}-darwin-arm64.tar.gz` |
| macOS | Intel | `pluginator-{version}-darwin-x64.tar.gz` |
| Linux | x64 | `pluginator-{version}-linux-x64.tar.gz` |
| Linux | ARM64 | `pluginator-{version}-linux-arm64.tar.gz` |
| Windows | x64 | `pluginator-{version}-windows-x64.zip` |

### Manual Binary Installation

#### macOS/Linux

```bash
# Download (replace version and platform)
curl -LO https://github.com/NindroidA/pluginator/releases/latest/download/pluginator-2.3.0-linux-x64.tar.gz

# Extract
tar -xzf pluginator-2.3.0-linux-x64.tar.gz

# Move to PATH
sudo mv pluginator-* /usr/local/bin/pluginator

# Verify
pluginator --version
```

#### Windows

1. Download the `-setup.exe` installer and run it, **or**:
2. Download `pluginator-{version}-windows-x64.zip`
3. Extract to a folder (e.g., `C:\Program Files\Pluginator`)
4. Add the folder to your PATH
5. Open a new terminal and run `pluginator --version`

## Verifying Installation

```bash
pluginator --version
```

Expected output:
```
pluginator v2.3.0
```

## Requirements

### Runtime Requirements

- **npm/Bun install**: Bun v1.1.0+ or Node.js 18+
- **Binary install**: No runtime requirements (standalone)

### System Requirements

- **OS**: macOS 11+, Windows 10+, Linux (glibc 2.17+)
- **Architecture**: x64 or ARM64
- **Terminal**: 140 columns x 40 rows minimum
- **Network**: Internet connection for update checks

## Updating

See [AUTO_UPDATER.md](systems/AUTO_UPDATER.md) for update instructions.

Quick reference:

| Install Method | Update Command |
|----------------|----------------|
| npm | `npm update -g pluginator` |
| Bun | `bun update -g pluginator` |
| Homebrew | `brew upgrade pluginator` |
| Scoop | `scoop update pluginator` |
| .deb | Download new `.deb` from releases |
| Binary | Re-run install script or download new release |

## Uninstalling

### npm

```bash
npm uninstall -g pluginator
```

### Bun

```bash
bun remove -g pluginator
```

### Homebrew

```bash
brew uninstall pluginator
```

### Scoop

```bash
scoop uninstall pluginator
```

### .deb (Linux)

```bash
sudo dpkg -r pluginator
```

### Binary

```bash
sudo rm /usr/local/bin/pluginator
```

### User Data

User data at `~/.pluginator/` is preserved by default during uninstall. To also remove it:

```bash
# Via the CLI (recommended)
pluginator uninstall --remove-data

# Or manually
rm -rf ~/.pluginator
```

## Troubleshooting

### "command not found"

Ensure the install location is in your PATH:

```bash
# Check if pluginator is in PATH
which pluginator

# For npm global installs, ensure npm bin is in PATH
export PATH="$PATH:$(npm bin -g)"
```

### Permission denied

For system-wide installs, use `sudo`:

```bash
sudo npm install -g pluginator
```

Or install to user directory:

```bash
npm install -g pluginator --prefix ~/.local
```

### Homebrew tap not found

```bash
# Re-add the tap
brew tap nindroida/pluginator

# Update Homebrew
brew update
```

### Binary won't run on macOS

macOS may quarantine downloaded binaries:

```bash
# Remove quarantine attribute
xattr -d com.apple.quarantine /usr/local/bin/pluginator
```

### Linux: GLIBC version error

The binary requires glibc 2.17+. Check your version:

```bash
ldd --version
```

If your glibc is older, use the npm/Bun installation method instead.
