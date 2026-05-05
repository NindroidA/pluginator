# Security Documentation

> **Last Updated:** April 30, 2026

This document outlines security considerations, best practices, and threat mitigations in Pluginator.

## Security Model

Pluginator operates with the following security principles:

1. **Least Privilege** - Only requests permissions needed for operation
2. **Defense in Depth** - Multiple layers of validation and protection
3. **Fail Secure** - Errors default to safe/conservative behavior
4. **No Remote Code Execution** - Never evaluates arbitrary code

## Threat Model

### In Scope

- Path traversal attacks
- Malicious file downloads
- Configuration injection
- API credential exposure
- File integrity verification
- SSRF via plugin source URLs (v2.8.0+)
- File corruption on crash (atomic write boundary, v2.8.0+)

### Out of Scope

- Operating system vulnerabilities
- Network-level attacks (MITM without HTTPS)
- Physical access to the server
- Vulnerabilities in downloaded plugins themselves

## Security Features

### Path Traversal Prevention

All file operations use safe path utilities:

```typescript
// Safe path joining prevents traversal
const result = safePath('/base/dir', '../../../etc/passwd');
// result.valid === false
// result.error === 'Path traversal detected'

// Safe join throws on traversal attempts
safeJoin('/server/plugins', '../../../sensitive'); // Throws Error
```

Protected operations:
- Plugin file copying
- Backup creation
- Configuration file access
- Log file writing

### Filename Sanitization

Downloaded files have their names sanitized:

```typescript
sanitizeFilename('plugin<script>.jar');     // 'pluginscript.jar'
sanitizeFilename('../../malicious.jar');    // 'malicious.jar'
sanitizeFilename('file:with:colons.jar');   // 'filewithcolons.jar'
```

Removed characters:
- Path separators (`/`, `\`)
- Special characters (`<`, `>`, `:`, `"`, `|`, `?`, `*`)
- Control characters (`\x00` - `\x1f`)
- Leading dots

### Checksum Verification

All downloads can be verified against checksums:

```typescript
const result = await verifyChecksum(filePath, expectedHash, 'sha256');
if (!result.valid) {
  // Delete the file, report error
}
```

Supported algorithms:
- SHA-256 (recommended)
- SHA-512
- SHA-1 (legacy)
- MD5 (legacy, not recommended)

### SSRF Prevention (v2.8.0+)

All outbound HTTP requests routed through `BasePluginSource.fetchJson` are validated by [src/infrastructure/url-security.ts](src/infrastructure/url-security.ts) before connecting. The validator (`validateExternalUrl`) raises a `SecurityError` (PLG-8001 through PLG-8004) for any of the following:

- **Private IPv4 ranges**: `10.0.0.0/8`, `172.16.0.0/12`, `192.168.0.0/16`, `169.254.0.0/16` (link-local / cloud metadata), `0.0.0.0/8`, `127.0.0.0/8`
- **Private IPv6 ranges**: `fc00::/7` (unique-local), `fe80::/10` (link-local), `::1` (loopback in any form)
- **IPv4-mapped/compatible IPv6 bypass attempts**: `::ffff:x.x.x.x`, `::x.x.x.x`
- **Numeric IP encodings**: hex (`0x7f000001`), decimal (`2130706433`), octal (`0177.0.0.1`)
- **Blocked hostnames**: `localhost`, `0.0.0.0`, `[::]`, and IPv4-mapped variants
- **Disallowed protocols**: anything other than `http:` or `https:` (blocks `file://`, `ftp://`, `gopher://`, etc.)

This protects against attackers who can influence a source URL (e.g., custom web manifests, Jenkins URLs, theme marketplace URLs, OAuth callback redirects) from probing internal services or cloud metadata endpoints.

### HTTPS Enforcement

All built-in plugin source APIs use HTTPS:
- Spigot/Spiget API
- Modrinth API
- GitHub API
- CurseForge API
- Hangar API (PaperMC)
- GeyserMC API
- Jenkins (HTTPS preferred; HTTP allowed for self-hosted CI)
- Web manifests (HTTP/HTTPS, validated as above)

### Credential Management

**Strong recommendation: store API tokens in environment variables, never in shell history or config files committed to version control.**

```bash
# GitHub personal access token (60/hr → 5000/hr rate limit lift)
export GITHUB_TOKEN=ghp_xxxxx

# CurseForge API key (paid tier required)
export CURSEFORGE_API_KEY=$2a$10$xxxxx

# NinSys account token (set automatically by `pluginator login`)
# Stored encrypted in ~/.pluginator/session.json
```

**Token handling rules (v2.8.0+ guidance):**
- CLI flags that take tokens are accepted but discouraged — they leak into shell history and `ps` output
- Environment variables take precedence over config files
- If a token must live in a config file, that file should be `chmod 600`
- **Never commit credentials to version control.** Pluginator does not write tokens to logs (logs are sanitized — see Logging Security below).

### Session Token Encryption (v1.7.0+)

Session tokens are encrypted at rest to provide defense-in-depth protection:

**Encryption Details:**
- Algorithm: AES-256-GCM (authenticated encryption)
- Key derivation: PBKDF2 with 100,000 iterations (SHA-256)
- IV: 12 bytes (96 bits) per NIST recommendation
- Auth tag: 16 bytes (128 bits) for tamper detection

**Key Material:**
- Derived from machine-specific identifier (hostname, username, home directory, architecture)
- Salt stored in `~/.pluginator/.salt` with 0o600 permissions
- Key buffers are zeroed after use to minimize memory exposure

**Session File Format (v2):**
```json
{
  "version": 2,
  "iv": "<base64-encoded-12-byte-iv>",
  "authTag": "<base64-encoded-16-byte-tag>",
  "ciphertext": "<base64-encoded-encrypted-session>"
}
```

**Threat Mitigations:**
| Threat | Mitigation |
|--------|------------|
| Malware reading session file | Token encrypted, not readable without key |
| Backup exposure | Tokens remain encrypted in backups |
| Session file moved/copied | Encryption tied to machine identity |
| Forensic recovery | Deleted plaintext not recoverable |
| Memory scraping | Key buffers zeroed after use |

**Backward Compatibility:**
- Plaintext sessions (v1) are still readable
- Automatically upgraded to encrypted (v2) on next save
- No manual migration required

### Atomic File Writes (v2.8.0+)

State files that, if corrupted on crash, would leave the user unable to launch Pluginator are written via [src/infrastructure/atomic-write.ts](src/infrastructure/atomic-write.ts) (`writeFileAtomic` / `writeFileAtomicSync`). The pattern is **write-to-`.tmp`-then-rename**, relying on POSIX rename atomicity within a single filesystem.

Files protected by atomic write:
- `~/.pluginator/config.json`
- `~/.pluginator/session.json`
- `~/.pluginator/preferences.json`
- `~/.pluginator/scan-cache.json`
- `~/.pluginator/notifications.json`
- `~/.pluginator/schedule.json`
- `~/.pluginator/themes/active.json`
- Operation journal entries
- Server config snapshots
- Sync copy targets (JAR copies write `.tmp` then rename)

Stale `.tmp` files from prior crashes are cleaned up on startup by `cleanStaleTempFiles`.

### Logger Race-Condition Fix (v2.11.52+)

The logger's `readOnlyFiles` option (which chmod-flipped log files 444↔644 on every write to deter tampering) was changed to default `false` in v2.11.52 after diagnostic logging revealed the chmod-back race window was causing intermittent `EACCES` failures during high-frequency log writes — these were being swallowed by upstream `.catch(() => [])` handlers and silently breaking unrelated workflows (e.g., plugin scans returning 0 plugins). Tamper-resistance of logs at rest was judged not worth the runtime cost. See [src/infrastructure/logger.ts](src/infrastructure/logger.ts) for the rationale comment.

**CLI Commands:**
```bash
# Check session encryption status
pluginator auth status

# Manually migrate to encrypted format
pluginator auth migrate

# Clear session (logout)
pluginator auth clear
```

## Configuration Security

### File Permissions

Recommended permissions:

```bash
# User data directory
chmod 700 ~/.pluginator/

# Configuration files
chmod 600 ~/.pluginator/config.json
chmod 600 ~/.pluginator/session.json

# Data directories
chmod 700 ~/.pluginator/backups/
chmod 700 ~/.pluginator/logs/
chmod 700 ~/.pluginator/cache/
```

### Input Validation

All configuration is validated with Zod schemas:

```typescript
// Paths are validated
PROD_SERVER_PATH: z.string().min(1)

// Numbers have bounds
MAX_BACKUPS: z.number().int().min(1).max(100)

// URLs are validated
apiUrl: z.string().url()
```

### Environment Variable Precedence

Environment variables override config files, allowing secure CI/CD:

```bash
# Override without modifying files
PROD_SERVER_PATH=/secure/path pluginator sync
```

## API Security

### Rate Limiting

Built-in rate limiting prevents abuse:

| API | Rate Limit |
|-----|------------|
| Spigot (Spiget) | 100 req/min |
| Modrinth | 300 req/min |
| GitHub | 60/hr (5000/hr with token) |
| CurseForge | Varies by key tier |

### Request Timeouts

All HTTP requests have hardcoded timeouts (8-10 seconds) to prevent hanging connections.

### User-Agent Headers

Proper identification in requests:

The User-Agent header is dynamically set from `package.json` version (e.g., `Pluginator/2.11.61`).

### Error Cause Preservation (v2.11.1+)

`toPluginatorError()` in [src/core/errors/error-types.ts](src/core/errors/error-types.ts) preserves the original thrown value as the standard ES2022 `error.cause` property. Native Node errors (e.g., `TimeoutError`, `AbortError`, `ETIMEDOUT`, `ENOTFOUND`) are classified by their typed name/code first; substring matching is fallback-only. This prevents a misleading "this is a timeout" classification from masking a different underlying error.

## Backup Security

### Backup Integrity

Backups are tar.gz archives with predictable structure:
- No symlinks followed (prevents symlink attacks)
- No device files included
- No setuid/setgid bits preserved

### Backup Rotation

Automatic cleanup prevents disk exhaustion. Configure max backup count in `~/.pluginator/config.json`.

### Backup Location

Backups are stored in `~/.pluginator/backups/` by default.

## Logging Security

### Sensitive Data

Logs are sanitized to avoid exposing:
- API tokens
- Passwords
- Full file paths (when possible)

### Log Rotation

Log files are created daily and automatically cleaned up.

### Log Permissions

```bash
chmod 600 ~/.pluginator/logs/*.log
```

## Secure Development Practices

### Dependencies

- Minimal dependency footprint
- Regular dependency audits
- No arbitrary code execution (eval, etc.)

### Code Review

All changes undergo review for:
- Input validation
- Path handling
- Error handling
- Credential exposure

## Reporting Security Issues

**Do not report security vulnerabilities publicly.**

Contact: [Open a private security advisory](https://github.com/NindroidA/pluginator/security/advisories/new) or reach out via [GitHub profile](https://github.com/NindroidA)

Include:
1. Description of the vulnerability
2. Steps to reproduce
3. Potential impact
4. Suggested fix (if any)

Response timeline:
- Acknowledgment: 48 hours
- Assessment: 7 days
- Fix (if confirmed): 30 days

## Security Checklist for Users

### Initial Setup

- [ ] Set restrictive permissions on config files
- [ ] Use environment variables for API tokens
- [ ] Verify server paths are correct
- [ ] Enable HTTPS for custom web manifests

### Regular Maintenance

- [ ] Keep Pluginator updated
- [ ] Review logs for anomalies
- [ ] Audit plugin sources periodically
- [ ] Verify backup integrity

### Production Deployment

- [ ] Run as non-root user
- [ ] Use dedicated service account
- [ ] Restrict network access if possible
- [ ] Monitor for unauthorized changes

## Known Limitations

1. **Plugin Content** - Pluginator cannot verify the safety of plugin contents
2. **Source Trust** - Relies on source repositories for plugin safety
3. **Network Security** - Assumes secure network between client and APIs
4. **Local Access** - Cannot protect against malicious local users

## Compliance Notes

Pluginator is designed to support:
- General data protection best practices
- Secure software development lifecycle
- Industry-standard encryption (HTTPS/TLS)

For specific compliance requirements (GDPR, SOC2, etc.), additional measures may be needed at the deployment level.
