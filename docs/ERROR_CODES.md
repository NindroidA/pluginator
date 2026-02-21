# Pluginator Error Codes

> **Last Updated:** February 21, 2026
> **Version:** 2.2.10

This document describes all error codes used by Pluginator. Each error includes its PLG code, string code, when it occurs, and how to resolve it.

---

## Looking Up Error Codes

When Pluginator displays an error, it includes a **PLG code** in the format `PLG-NNNN`:

```
[PLG-1001] Request timed out. Please try again.
```

Use the PLG code to quickly find the error in this document. The numbering follows category ranges:

| Range | Category | Description |
|-------|----------|-------------|
| PLG-1000s | Network | HTTP, connectivity issues |
| PLG-2000s | Validation | Config, schema validation |
| PLG-3000s | Filesystem | File read/write, permissions |
| PLG-4000s | Source | Plugin source API errors |
| PLG-5000s | Auth | Authentication, authorization |
| PLG-6000s | Config | Configuration parsing |
| PLG-7000s | Workflow | Sync, download, update failures |
| PLG-8000s | Security | URL validation, SSRF prevention |

---

## Quick Reference

| PLG Code | Type | String Code | Summary |
|----------|------|-------------|---------|
| PLG-1001 | network | TIMEOUT | Request timed out. |
| PLG-1002 | network | CONNECTION_FAILED | Could not connect to the server. |
| PLG-1003 | network | RATE_LIMITED | Too many requests. |
| PLG-1004 | network | SERVER_ERROR | The server encountered an error. |
| PLG-1005 | network | OFFLINE | You appear to be offline. |
| PLG-2001 | validation | INVALID_CONFIG | Invalid configuration. |
| PLG-2002 | validation | INVALID_PLUGIN | Invalid plugin format. |
| PLG-2003 | validation | SCHEMA_MISMATCH | Data format mismatch. |
| PLG-2004 | validation | MISSING_FIELD | Required field missing. |
| PLG-3001 | filesystem | FILE_NOT_FOUND | File not found. |
| PLG-3002 | filesystem | PERMISSION_DENIED | Permission denied. |
| PLG-3003 | filesystem | DISK_FULL | Disk is full. |
| PLG-3004 | filesystem | PATH_EXISTS | Path already exists. |
| PLG-3005 | filesystem | WRITE_FAILED | Failed to write file. |
| PLG-4001 | source | API_ERROR | Failed to fetch data from source. |
| PLG-4002 | source | PLUGIN_NOT_FOUND | Plugin not found. |
| PLG-4003 | source | VERSION_MISMATCH | Version compatibility issue. |
| PLG-4004 | source | UNSUPPORTED_SOURCE | Source type not supported. |
| PLG-5001 | auth | NOT_AUTHENTICATED | Please log in to continue. |
| PLG-5002 | auth | TOKEN_EXPIRED | Session has expired. |
| PLG-5003 | auth | INSUFFICIENT_TIER | Requires higher subscription tier. |
| PLG-5004 | auth | INVALID_CREDENTIALS | Invalid email or password. |
| PLG-6001 | config | PARSE_ERROR | Failed to parse configuration. |
| PLG-6002 | config | INVALID_VALUE | Invalid value. |
| PLG-6003 | config | MISSING_REQUIRED | Missing required setting. |
| PLG-6004 | config | UNKNOWN_OPTION | Unknown configuration option. |
| PLG-7001 | workflow | SYNC_FAILED | Failed to sync plugins. |
| PLG-7002 | workflow | DOWNLOAD_FAILED | Failed to download plugin. |
| PLG-7003 | workflow | UPDATE_CHECK_FAILED | Failed to check for updates. |
| PLG-7004 | workflow | BACKUP_FAILED | Failed to create backup. |
| PLG-7005 | workflow | ROLLBACK_FAILED | Failed to rollback. |
| PLG-8001 | security | INTERNAL_ADDRESS | Internal addresses not allowed. |
| PLG-8002 | security | PRIVATE_IP | Private IP ranges not allowed. |
| PLG-8003 | security | INVALID_PROTOCOL | Only HTTP/HTTPS allowed. |
| PLG-8004 | security | INVALID_URL | Invalid URL format. |

---

## Error System Overview

Pluginator uses a typed error system with:
- **PLG codes** (`PLG-NNNN`) for user-facing identification and documentation lookup
- **Discriminated types** for type-safe error handling
- **String error codes** for programmatic identification
- **User-friendly messages** via `getUserMessage()`
- **Retryability flags** to indicate if retry might succeed
- **Context objects** for debugging information

### Error Types

| Type | PLG Range | Description | Default Retryable |
|------|-----------|-------------|-------------------|
| `network` | 1000s | HTTP, connectivity issues | Yes |
| `validation` | 2000s | Config, schema validation | No |
| `filesystem` | 3000s | File read/write, permissions | No |
| `source` | 4000s | Plugin source API errors | Yes |
| `auth` | 5000s | Authentication, authorization | No |
| `config` | 6000s | Configuration parsing | No |
| `workflow` | 7000s | Sync, download, update failures | Yes |
| `security` | 8000s | URL validation, SSRF prevention | No |

---

## Network Errors

Network errors occur during HTTP requests to plugin sources or APIs.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-1001 | `TIMEOUT` | Request exceeded timeout limit | Yes |
| PLG-1002 | `CONNECTION_FAILED` | Could not establish connection | Yes |
| PLG-1003 | `RATE_LIMITED` | Too many requests (HTTP 429) | Yes |
| PLG-1004 | `SERVER_ERROR` | Server returned 5xx status | Yes |
| PLG-1005 | `OFFLINE` | No network connectivity | Yes |

### PLG-1001 — TIMEOUT

**When it occurs:**
- HTTP request takes longer than configured timeout (default 30s)
- Server is slow to respond

**User Message:** `[PLG-1001] Request timed out. Please try again.`

**Resolution:**
- Check your internet connection
- Try again later when the server may be less busy
- Increase timeout in configuration if consistently slow

**Context Fields:**
- `url`: The URL that timed out
- `statusCode`: May be undefined if connection never established

### PLG-1002 — CONNECTION_FAILED

**When it occurs:**
- DNS resolution fails
- Server refuses connection
- Network route unavailable

**User Message:** `[PLG-1002] Could not connect to the server. Check your internet connection.`

**Resolution:**
- Verify internet connectivity
- Check if the service is down
- Verify firewall settings

### PLG-1003 — RATE_LIMITED

**When it occurs:**
- Too many requests to a plugin source
- API rate limit exceeded

**User Message:** `[PLG-1003] Too many requests. Please wait before trying again.`

**Resolution:**
- Wait before retrying (usually 1-5 minutes)
- For GitHub, add a personal access token for higher limits
- Reduce frequency of update checks

**Context Fields:**
- `statusCode`: 429
- `retryAfter`: Seconds to wait (if provided by server)

### PLG-1004 — SERVER_ERROR

**When it occurs:**
- Server returns HTTP 500-599 status
- Backend service failure

**User Message:** `[PLG-1004] The server encountered an error. Please try again later.`

**Resolution:**
- Wait and retry
- Check service status pages
- Report if persistent

**Context Fields:**
- `statusCode`: The HTTP status code (500, 502, 503, etc.)

### PLG-1005 — OFFLINE

**When it occurs:**
- No network interface available
- Network adapter disabled

**User Message:** `[PLG-1005] You appear to be offline. Check your internet connection.`

**Resolution:**
- Enable network connection
- Check WiFi/Ethernet status

---

## Validation Errors

Validation errors occur when data doesn't match expected formats.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-2001 | `INVALID_CONFIG` | Configuration file has invalid structure | No |
| PLG-2002 | `INVALID_PLUGIN` | Plugin JAR is malformed or corrupt | No |
| PLG-2003 | `SCHEMA_MISMATCH` | Data doesn't match expected schema | No |
| PLG-2004 | `MISSING_FIELD` | Required field is missing | No |

### PLG-2001 — INVALID_CONFIG

**When it occurs:**
- `config.json` contains invalid values
- Configuration doesn't pass Zod validation

**User Message:** `[PLG-2001] Invalid configuration (field: <field>). Please check your settings.`

**Resolution:**
- Check `~/.pluginator/config.json` for errors
- Refer to configuration documentation
- Delete config file to reset to defaults

**Context Fields:**
- `field`: The invalid field name

### PLG-2002 — INVALID_PLUGIN

**When it occurs:**
- JAR file is corrupt or truncated
- Invalid ZIP structure
- Missing required files inside JAR

**User Message:** `[PLG-2002] Invalid plugin format (field: <field>). The plugin may be corrupted.`

**Resolution:**
- Re-download the plugin
- Check if download was interrupted
- Verify plugin compatibility with your server version

### PLG-2003 — SCHEMA_MISMATCH

**When it occurs:**
- API response format changed
- Cached data from incompatible version

**User Message:** `[PLG-2003] Data format mismatch (field: <field>). This may be a compatibility issue.`

**Resolution:**
- Clear cache: `rm -rf ~/.pluginator/cache`
- Update Pluginator to latest version

### PLG-2004 — MISSING_FIELD

**When it occurs:**
- Required configuration field not provided
- Plugin manifest missing required properties

**User Message:** `[PLG-2004] Required field missing (field: <field>).`

**Resolution:**
- Add the missing field to your configuration
- Check plugin.yml in your plugin JAR

**Context Fields:**
- `field`: The missing field name

---

## Filesystem Errors

Filesystem errors occur during file operations.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-3001 | `FILE_NOT_FOUND` | File doesn't exist at expected path | No |
| PLG-3002 | `PERMISSION_DENIED` | Insufficient permissions to access file | No |
| PLG-3003 | `DISK_FULL` | No space left on device | No |
| PLG-3004 | `PATH_EXISTS` | Path already exists when creating | No |
| PLG-3005 | `WRITE_FAILED` | Failed to write to file | No |

### PLG-3001 — FILE_NOT_FOUND

**When it occurs:**
- Plugin JAR doesn't exist at scanned path
- Configuration file missing

**User Message:** `[PLG-3001] File not found: <path>.`

**Resolution:**
- Verify the file path
- Check if file was moved or deleted
- Run scan to refresh plugin list

**Context Fields:**
- `path`: The missing file path

### PLG-3002 — PERMISSION_DENIED

**When it occurs:**
- No read/write access to plugins directory
- Trying to modify system files

**User Message:** `[PLG-3002] Permission denied: <path>. Check file permissions.`

**Resolution:**
- Run with appropriate permissions
- Check directory ownership
- Verify user has access to the plugins folder

**Context Fields:**
- `path`: The inaccessible path

### PLG-3003 — DISK_FULL

**When it occurs:**
- No space available for downloads
- No space for backups

**User Message:** `[PLG-3003] Disk is full. Free up some space and try again.`

**Resolution:**
- Free disk space
- Clean up old backups: `~/.pluginator/backups/`
- Clear cache: `~/.pluginator/cache/`

### PLG-3004 — PATH_EXISTS

**When it occurs:**
- Trying to create a file that already exists
- Backup target already exists

**User Message:** `[PLG-3004] Path already exists: <path>.`

**Resolution:**
- Remove existing file first
- Choose a different name

**Context Fields:**
- `path`: The existing path

### PLG-3005 — WRITE_FAILED

**When it occurs:**
- I/O error during write
- File locked by another process

**User Message:** `[PLG-3005] Failed to write file: <path>.`

**Resolution:**
- Check if file is open in another program
- Verify disk is working properly
- Check available disk space

**Context Fields:**
- `path`: The file that couldn't be written

---

## Source Errors

Source errors occur when interacting with plugin repositories.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-4001 | `API_ERROR` | Plugin source API returned error | Yes |
| PLG-4002 | `PLUGIN_NOT_FOUND` | Plugin doesn't exist in source | No |
| PLG-4003 | `VERSION_MISMATCH` | Requested version unavailable | No |
| PLG-4004 | `UNSUPPORTED_SOURCE` | Source type not implemented | No |

### PLG-4001 — API_ERROR

**When it occurs:**
- Modrinth, Spigot, or other API returns error
- API structure changed unexpectedly

**User Message:** `[PLG-4001] Failed to fetch data from <source>. The service may be temporarily unavailable.`

**Resolution:**
- Wait and retry
- Check source's status page
- Report if persistent

**Context Fields:**
- `source`: The plugin source name (modrinth, spigot, github, etc.)
- `statusCode`: HTTP status if available

### PLG-4002 — PLUGIN_NOT_FOUND

**When it occurs:**
- Plugin ID/slug not found in source
- Plugin was removed from repository
- Typo in plugin identifier

**User Message:** `[PLG-4002] Plugin not found from <source>. Check the plugin name or ID.`

**Resolution:**
- Verify the plugin ID/slug is correct
- Check if the plugin still exists on the source
- Try searching by name instead of ID

**Context Fields:**
- `source`: The plugin source
- `pluginId`: The requested plugin identifier

### PLG-4003 — VERSION_MISMATCH

**When it occurs:**
- Requested version doesn't exist
- Version incompatible with Minecraft version

**User Message:** `[PLG-4003] Version compatibility issue from <source>.`

**Resolution:**
- Check available versions for your Minecraft version
- Update Minecraft server version
- Use a different plugin version

**Context Fields:**
- `source`: The plugin source
- `requestedVersion`: The version that wasn't found

### PLG-4004 — UNSUPPORTED_SOURCE

**When it occurs:**
- Plugin configured with unknown source type
- Source type deprecated

**User Message:** `[PLG-4004] Source type not supported: <source>.`

**Resolution:**
- Check supported sources: modrinth, spigot, github, curseforge, hangar, jenkins, bukkit, web-manifest
- Update plugin configuration to use supported source

**Context Fields:**
- `source`: The unsupported source type

---

## Auth Errors

Authentication errors occur with account operations.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-5001 | `NOT_AUTHENTICATED` | No active session | No |
| PLG-5002 | `TOKEN_EXPIRED` | Session token expired | No |
| PLG-5003 | `INSUFFICIENT_TIER` | Feature requires higher tier | No |
| PLG-5004 | `INVALID_CREDENTIALS` | Wrong email/password | No |

### PLG-5001 — NOT_AUTHENTICATED

**When it occurs:**
- Trying to access tier-gated feature without login
- Session file deleted

**User Message:** `[PLG-5001] Please log in to continue.`

**Resolution:**
- Run `/login` command
- Use email, Google, or GitHub authentication

### PLG-5002 — TOKEN_EXPIRED

**When it occurs:**
- Session older than 24 hours
- Server invalidated token

**User Message:** `[PLG-5002] Your session has expired. Please log in again.`

**Resolution:**
- Run `/login` to re-authenticate

### PLG-5003 — INSUFFICIENT_TIER

**When it occurs:**
- Accessing Pro/Max features on Free/Plus tier
- Exceeding tier limits

**User Message:** `[PLG-5003] This feature requires a higher subscription tier.`

**Resolution:**
- Upgrade your subscription
- Use `/account` to see current tier and limits

### PLG-5004 — INVALID_CREDENTIALS

**When it occurs:**
- Wrong email or password
- Account doesn't exist

**User Message:** `[PLG-5004] Invalid email or password.`

**Resolution:**
- Check email and password
- Reset password if forgotten
- Create account if new user

---

## Config Errors

Configuration errors occur when parsing or validating settings.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-6001 | `PARSE_ERROR` | JSON/YAML syntax error | No |
| PLG-6002 | `INVALID_VALUE` | Value out of range or wrong type | No |
| PLG-6003 | `MISSING_REQUIRED` | Required setting not provided | No |
| PLG-6004 | `UNKNOWN_OPTION` | Unrecognized configuration key | No |

### PLG-6001 — PARSE_ERROR

**When it occurs:**
- Malformed JSON in config file
- Syntax error in configuration

**User Message:** `[PLG-6001] Failed to parse configuration for <setting>. Check the file format.`

**Resolution:**
- Validate JSON syntax
- Check for trailing commas, missing quotes
- Use a JSON linter

**Context Fields:**
- `setting`: The problematic setting

### PLG-6002 — INVALID_VALUE

**When it occurs:**
- Number out of expected range
- Enum value not in allowed list

**User Message:** `[PLG-6002] Invalid value for <setting>.`

**Resolution:**
- Check documentation for valid values
- Reset to default if unsure

**Context Fields:**
- `setting`: The invalid setting

### PLG-6003 — MISSING_REQUIRED

**When it occurs:**
- Essential configuration not provided
- Server path not configured

**User Message:** `[PLG-6003] Missing required setting <setting>.`

**Resolution:**
- Add the missing setting to configuration
- Run setup wizard to configure

**Context Fields:**
- `setting`: The missing setting name

### PLG-6004 — UNKNOWN_OPTION

**When it occurs:**
- Typo in configuration key
- Option deprecated or renamed

**User Message:** `[PLG-6004] Unknown configuration option <setting>.`

**Resolution:**
- Check spelling of configuration key
- Remove unknown options
- Check changelog for renamed settings

**Context Fields:**
- `setting`: The unknown option name

---

## Workflow Errors

Workflow errors occur during multi-step operations.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-7001 | `SYNC_FAILED` | Plugin sync operation failed | Yes |
| PLG-7002 | `DOWNLOAD_FAILED` | Plugin download failed | Yes |
| PLG-7003 | `UPDATE_CHECK_FAILED` | Could not check for updates | Yes |
| PLG-7004 | `BACKUP_FAILED` | Backup creation failed | Yes |
| PLG-7005 | `ROLLBACK_FAILED` | Could not restore previous version | Yes |

### PLG-7001 — SYNC_FAILED

**When it occurs:**
- Copying plugins between servers failed
- File copy error during sync

**User Message:** `[PLG-7001] Failed to sync plugins. Some files may not have been copied.`

**Resolution:**
- Check source and destination paths
- Verify permissions on both servers
- Try syncing individual plugins

**Context Fields:**
- `operation`: "sync"
- `sourceServer`: Source server ID
- `targetServer`: Target server ID

### PLG-7002 — DOWNLOAD_FAILED

**When it occurs:**
- Plugin download interrupted
- Checksum verification failed
- Server returned error during download

**User Message:** `[PLG-7002] Failed to download plugin. Please try again.`

**Resolution:**
- Check internet connection
- Verify plugin is still available
- Try alternative source

**Context Fields:**
- `operation`: "download"
- `pluginName`: Name of plugin being downloaded

### PLG-7003 — UPDATE_CHECK_FAILED

**When it occurs:**
- Could not reach update API
- Error parsing update response

**User Message:** `[PLG-7003] Failed to check for updates. Please try again.`

**Resolution:**
- Check internet connection
- Try again later
- Check if specific source is down

**Context Fields:**
- `operation`: "update-check"

### PLG-7004 — BACKUP_FAILED

**When it occurs:**
- Could not create backup archive
- No space for backup

**User Message:** `[PLG-7004] Failed to create backup.`

**Resolution:**
- Check available disk space
- Verify backup directory permissions
- Check `~/.pluginator/backups/` accessibility

**Context Fields:**
- `operation`: "backup"

### PLG-7005 — ROLLBACK_FAILED

**When it occurs:**
- Backup file corrupt or missing
- Permission denied during restore
- Version incompatibility

**User Message:** `[PLG-7005] Failed to rollback. Manual intervention may be required.`

**Resolution:**
- Check if backup exists
- Manually restore from backup directory
- Re-download previous version

**Context Fields:**
- `operation`: "rollback"
- `pluginName`: Plugin being rolled back
- `backupPath`: Path to backup file

---

## Security Errors

Security errors prevent potentially dangerous operations.

| PLG Code | Code | Description | Retryable |
|----------|------|-------------|-----------|
| PLG-8001 | `INTERNAL_ADDRESS` | URL points to internal service | No |
| PLG-8002 | `PRIVATE_IP` | URL resolves to private IP range | No |
| PLG-8003 | `INVALID_PROTOCOL` | Non-HTTP/HTTPS protocol | No |
| PLG-8004 | `INVALID_URL` | URL format is invalid | No |

### PLG-8001 — INTERNAL_ADDRESS

**When it occurs:**
- URL points to localhost, 127.x.x.x
- URL uses internal hostname

**User Message:** `[PLG-8001] Internal addresses are not allowed for external sources.`

**Resolution:**
- Use public URLs only
- For local Jenkins, configure as local source type

**Context Fields:**
- `url`: The blocked URL

### PLG-8002 — PRIVATE_IP

**When it occurs:**
- URL resolves to 10.x.x.x, 172.16-31.x.x, 192.168.x.x
- SSRF prevention triggered

**User Message:** `[PLG-8002] Private IP ranges are not allowed for external sources.`

**Resolution:**
- Use public URLs only
- Configure private sources as local type

**Context Fields:**
- `url`: The blocked URL

### PLG-8003 — INVALID_PROTOCOL

**When it occurs:**
- Using FTP, file://, or other protocols
- Non-HTTP scheme in URL

**User Message:** `[PLG-8003] Only HTTP and HTTPS protocols are allowed.`

**Resolution:**
- Use HTTPS URLs
- HTTP is allowed but HTTPS preferred

**Context Fields:**
- `url`: The blocked URL

### PLG-8004 — INVALID_URL

**When it occurs:**
- Malformed URL string
- Missing required URL components

**User Message:** `[PLG-8004] Invalid URL format.`

**Resolution:**
- Check URL syntax
- Ensure URL includes protocol (https://)

**Context Fields:**
- `url`: The invalid URL string

---

## Programmatic Error Handling

### Type Guards

```typescript
import {
  isPluginatorError,
  isNetworkError,
  isRetryableError,
} from './core/errors';

try {
  await downloadPlugin(plugin);
} catch (error) {
  if (isNetworkError(error) && error.code === 'RATE_LIMITED') {
    // Wait and retry
    await sleep(60000);
    return downloadPlugin(plugin);
  }

  if (isRetryableError(error)) {
    // Generic retry logic
    return retry(() => downloadPlugin(plugin), { maxAttempts: 3 });
  }

  // Non-retryable error
  throw error;
}
```

### PLG Code Lookup

```typescript
import { getPlgCode, parsePlgCode } from './core/errors';

// Get PLG code for an error type + code
const plg = getPlgCode('network', 'TIMEOUT'); // 'PLG-1001'

// Parse a PLG code back to type + code
const info = parsePlgCode('PLG-1001'); // { type: 'network', code: 'TIMEOUT' }

// Access PLG code from an error instance
const error = new NetworkError('test', 'TIMEOUT');
console.log(error.plgCode); // 'PLG-1001'
console.log(error.getUserMessage()); // '[PLG-1001] Request timed out. Please try again.'
```

### Converting Unknown Errors

```typescript
import { toPluginatorError, getUserErrorMessage } from './core/errors';

try {
  await riskyOperation();
} catch (error) {
  const typedError = toPluginatorError(error);
  console.error(getUserErrorMessage(typedError));
}
```

### Workflow Results

For operations that may partially succeed:

```typescript
import { processBatch, type WorkflowResult } from './core/errors';

const result = await processBatch(
  plugins,
  (p) => p.name,
  async (plugin) => downloadPlugin(plugin),
  { continueOnError: true }
);

console.log(`${result.successCount}/${result.items.length} downloaded`);
for (const failure of result.failures) {
  console.error(`- ${failure.id}: ${failure.error.message}`);
}
```

---

## Reporting Issues

If you encounter an error that you believe is a bug:

1. Note the **PLG code** (e.g., `PLG-1001`) and the full error message
2. Check the log file: `~/.pluginator/logs/pluginator-YYYY-MM-DD.log`
3. Ask in the [NindroidSystems Discord](https://discord.gg/nkwMUaVSYH) for quick help
4. Open an issue at: https://github.com/NindroidA/pluginator/issues

Include:
- Pluginator version (`/version`)
- Operating system
- PLG error code and full message
- Steps to reproduce
- Relevant log entries
