# Troubleshooting Guide

> **Last Updated:** February 21, 2026

This guide helps you diagnose and resolve common issues with Pluginator.

## Quick Diagnostics

Run these commands to gather diagnostic information:

```bash
# Check version
pluginator --version

# Verify installation
which pluginator

# Check configuration
pluginator config

# Enable debug mode
PLUGINATOR_DEBUG=1 pluginator
```

## Common Issues

### Network Errors

#### Connection Timeout

**Symptoms:** Operations hang or fail with "timeout" error.

**Causes:**
- Slow network connection
- API server is down
- Firewall blocking connections

**Solutions:**

1. Check your internet connection
2. Try again later (API might be temporarily unavailable)
3. Check if you're behind a proxy:
   ```bash
   export HTTP_PROXY=http://proxy:port
   export HTTPS_PROXY=http://proxy:port
   ```

#### Rate Limiting

**Symptoms:** 429 errors, "Too Many Requests"

**Causes:**
- Exceeded API rate limits
- Too many concurrent requests

**Solutions:**

1. Wait before retrying (usually 1-5 minutes)
2. Enable caching in config
3. Check your tier limits with `/account`

#### SSL Certificate Errors

**Symptoms:** "unable to verify certificate" or "self-signed certificate"

**Solutions:**

1. Update your system certificates
2. If behind corporate proxy:
   ```bash
   export NODE_EXTRA_CA_CERTS=/path/to/corporate-ca.crt
   ```

### Authentication Errors

#### Token Expired

**Symptoms:** "Token expired" or "Unauthorized" errors

**Solutions:**

1. Re-login:
   ```bash
   pluginator login
   # or use /login in the UI
   ```

2. Clear session and re-authenticate:
   ```bash
   rm ~/.pluginator/session.json
   pluginator login
   ```

#### Invalid Credentials

**Symptoms:** "Invalid email or password"

**Solutions:**

1. Verify your credentials
2. Reset password at https://pluginator.nindroidsystems.com/reset-password
3. Try OAuth login (Google/GitHub) instead

### Sync Errors

#### Permission Denied

**Symptoms:** "EACCES: permission denied" or "EPERM: operation not permitted"

**Causes:**
- Insufficient file permissions
- Server running as different user
- Read-only file system

**Solutions:**

1. Check file ownership:
   ```bash
   ls -la /path/to/server/plugins/
   ```

2. Fix permissions:
   ```bash
   sudo chown -R $(whoami) /path/to/server/plugins/
   ```

3. Run with appropriate permissions (not recommended for production):
   ```bash
   sudo pluginator
   ```

#### Disk Full

**Symptoms:** "ENOSPC: no space left on device"

**Solutions:**

1. Check disk space:
   ```bash
   df -h
   ```

2. Clean up old backups:
   ```bash
   rm -rf ~/.pluginator/backups/*
   ```

3. Clean up cache:
   ```bash
   rm -rf ~/.pluginator/cache/*
   ```

#### File In Use

**Symptoms:** "EBUSY: resource busy" (Windows) or "Text file busy" (Linux)

**Causes:**
- Server is running with plugin loaded
- Antivirus scanning the file

**Solutions:**

1. Stop the Minecraft server before syncing
2. Temporarily disable antivirus
3. Wait and retry

### Update Errors

#### Version Mismatch

**Symptoms:** "Version comparison failed" or unexpected version detection

**Causes:**
- Non-standard version format
- Plugin using build numbers instead of semver

**Solutions:**

1. Check plugin configuration:
   ```bash
   cat ~/.pluginator/servers/<server-id>/plugins.json
   ```

2. Manually specify version format in config

#### Source Unavailable

**Symptoms:** "Source not responding" or specific API errors

**Causes:**
- Plugin source (Modrinth, Spigot, etc.) is down
- Plugin removed from source

**Solutions:**

1. Check source status:
   - Modrinth: https://status.modrinth.com
   - Spigot: https://www.spigotmc.org

2. Try alternative source if available

3. Temporarily disable update checking for that plugin

### UI Errors

#### Terminal Too Small

**Symptoms:** "Terminal too small" message

**Requirements:**
- Minimum: 140 columns x 40 rows

**Solutions:**

1. Resize terminal window
2. Reduce font size
3. Use fullscreen mode
4. Run without UI: `pluginator --no-ui`

#### Input Not Working

**Symptoms:** Keyboard input ignored, stuck in a state

**Solutions:**

1. Press `Ctrl+C` twice to force quit
2. Press `Escape` to close overlays
3. Check if in a dialog (may need Enter to confirm)

#### Colors/Rendering Issues

**Symptoms:** Garbled output, missing colors

**Causes:**
- Terminal doesn't support 256 colors
- TERM environment variable not set

**Solutions:**

1. Set terminal type:
   ```bash
   export TERM=xterm-256color
   ```

2. Try a different terminal emulator

3. Disable colors:
   ```bash
   NO_COLOR=1 pluginator
   ```

## Log Files

### Log Locations

```
~/.pluginator/logs/
├── pluginator-2026-01-25.log    # Daily log files
├── pluginator-2026-01-24.log
└── ...
```

### Enabling Debug Logs

```bash
# Set environment variable
export PLUGINATOR_DEBUG=1
pluginator

# Or inline
PLUGINATOR_DEBUG=1 pluginator
```

### Log Levels

- **ERROR** - Critical failures
- **WARN** - Non-critical issues
- **INFO** - General information
- **DEBUG** - Detailed debugging (requires PLUGINATOR_DEBUG=1)

### Reading Logs

```bash
# View today's log
cat ~/.pluginator/logs/pluginator-$(date +%Y-%m-%d).log

# Follow log in real-time
tail -f ~/.pluginator/logs/pluginator-$(date +%Y-%m-%d).log

# Search for errors
grep -i error ~/.pluginator/logs/pluginator-*.log
```

## Configuration Issues

### Reset Configuration

```bash
# Backup current config
cp ~/.pluginator/config.json ~/.pluginator/config.json.backup

# Remove config (will use defaults)
rm ~/.pluginator/config.json

# Re-run setup
pluginator setup
```

### Verify Configuration

```bash
# View current config
cat ~/.pluginator/config.json

# Check for JSON syntax errors
jq . ~/.pluginator/config.json
```

### Common Config Mistakes

1. **Invalid JSON syntax** - Missing commas, quotes
2. **Wrong path format** - Use forward slashes on all platforms
3. **Relative paths** - Always use absolute paths

## Debug Context Export

For complex issues, export debug context:

```bash
# In the UI, press ':' and type 'debug:export'
# Or use the command
pluginator debug-export
```

This creates a file in `.debug-context/` with:
- Environment info
- Application state
- Recent actions
- Error logs
- Configuration (sensitive values redacted)

## Platform-Specific Issues

### macOS

#### Gatekeeper Warning

**Symptoms:** "cannot be opened because the developer cannot be verified"

**Solutions:**

1. Allow in System Preferences > Security & Privacy
2. Or run:
   ```bash
   xattr -d com.apple.quarantine /usr/local/bin/pluginator
   ```

#### Sleep During Operations

If Mac sleeps during long operations, Pluginator uses `caffeinate` automatically. If issues persist:

```bash
# Verify caffeinate is working
caffeinate -i -w $$ &
```

### Linux

#### systemd-inhibit Not Found

Sleep prevention requires systemd. On non-systemd systems, operations continue but may be interrupted by sleep.

### Windows

#### PowerShell Execution Policy

**Symptoms:** Scripts fail to run

**Solutions:**

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

#### Path Too Long

**Symptoms:** "ENAMETOOLONG" errors

**Solutions:**

1. Enable long paths in Windows 10+:
   ```
   Computer Configuration > Administrative Templates > System > Filesystem > Enable Win32 long paths
   ```

2. Or use shorter installation path

## Getting Help

### Before Asking for Help

1. Check this troubleshooting guide
2. Ask in the [NindroidSystems Discord](https://discord.gg/nkwMUaVSYH) for quick help
3. Search [GitHub Issues](https://github.com/NindroidA/pluginator/issues)
4. Export debug context
5. Note the exact error message
6. Note your environment (OS, terminal, install method)

### Reporting Issues

File issues at: https://github.com/NindroidA/pluginator/issues

Include:
- Pluginator version (`pluginator --version`)
- Operating system
- Terminal application
- Steps to reproduce
- Expected vs actual behavior
- Debug context export (if applicable)
- Relevant log excerpts

## FAQ

### Q: Can I use Pluginator with multiple servers?

A: Yes! Add each server with `/scan` or in the Servers tab. Pluginator manages them independently.

### Q: Does Pluginator work offline?

A: Partially. You can view cached plugin info and manage local files, but update checking requires network access.

### Q: How do I skip certain plugins in sync?

A: Mark plugins with `disableOnTest: true` in the plugin config, or exclude them in sync settings.

### Q: Where are my backups stored?

A: In `~/.pluginator/backups/`. Configure backup retention in settings.

### Q: Can I run Pluginator headless (no UI)?

A: Yes, use CLI commands:
```bash
pluginator sync --yes
pluginator backup
pluginator check-updates --json
```

### Q: How do I update Pluginator itself?

A: Run `/update` in the UI or:
```bash
# npm
npm update -g pluginator

# bun
bun update -g pluginator

# Homebrew
brew upgrade pluginator
```

## See Also

- [USER_GUIDE.md](USER_GUIDE.md) - User documentation
- [Architecture Overview](architecture/OVERVIEW.md) - System architecture
- [CHANGELOG.md](CHANGELOG.md) - Version history
