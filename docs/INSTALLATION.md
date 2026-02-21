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

### Linux (APT - Debian/Ubuntu)

```bash
# Add repository
curl -fsSL https://pluginator.dev/apt/gpg.key | sudo gpg --dearmor -o /usr/share/keyrings/pluginator.gpg
echo "deb [signed-by=/usr/share/keyrings/pluginator.gpg] https://pluginator.dev/apt stable main" | sudo tee /etc/apt/sources.list.d/pluginator.list

# Install
sudo apt update
sudo apt install pluginator
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
curl -fsSL https://pluginator.dev/install.sh | bash
```

The script automatically detects your OS and architecture.

## Binary Downloads

Direct downloads are available at:
https://github.com/NindroidA/pluginator/releases/latest

### Available Binaries

| Platform | Architecture | Filename |
|----------|--------------|----------|
| macOS | Apple Silicon (M1/M2/M3) | `pluginator-darwin-arm64.tar.gz` |
| macOS | Intel | `pluginator-darwin-x64.tar.gz` |
| Linux | x64 | `pluginator-linux-x64.tar.gz` |
| Linux | ARM64 | `pluginator-linux-arm64.tar.gz` |
| Windows | x64 | `pluginator-win-x64.zip` |

### Manual Binary Installation

#### macOS/Linux

```bash
# Download (replace with your platform)
curl -LO https://github.com/NindroidA/pluginator/releases/latest/download/pluginator-linux-x64.tar.gz

# Extract
tar -xzf pluginator-linux-x64.tar.gz

# Move to PATH
sudo mv pluginator /usr/local/bin/

# Verify
pluginator --version
```

#### Windows

1. Download `pluginator-win-x64.zip`
2. Extract to a folder (e.g., `C:\Program Files\Pluginator`)
3. Add the folder to your PATH
4. Open a new terminal and run `pluginator --version`

## Verifying Installation

```bash
pluginator --version
```

Expected output:
```
pluginator v1.12.0
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
| APT | `sudo apt update && sudo apt upgrade pluginator` |
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

### APT

```bash
sudo apt remove pluginator
```

### Binary

```bash
sudo rm /usr/local/bin/pluginator
```

### User Data

To remove all user data:

```bash
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
