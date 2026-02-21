# Security Documentation

> **Last Updated:** February 21, 2026

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

### HTTPS Enforcement

All API communications use HTTPS:
- Spigot/Spiget API
- Modrinth API
- GitHub API
- CurseForge API
- Custom web manifests (configurable)

### Credential Management

API credentials are handled securely:

```ini
# Store in environment variables (preferred)
export GITHUB_TOKEN=ghp_xxxxx

# Or in config file with restricted permissions
# chmod 600 config/pluginator.config
GITHUB_TOKEN=ghp_xxxxx
```

**Never commit credentials to version control.**

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
# Configuration files
chmod 600 config/pluginator.config
chmod 600 config/theme.json

# Data directories
chmod 700 data/
chmod 700 data/backups/
chmod 700 data/logs/
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

All requests have configurable timeouts:

```ini
API_TIMEOUT=30  # seconds
```

### User-Agent Headers

Proper identification in requests:

```
User-Agent: Pluginator/1.0.0 (https://github.com/NindroidA/pluginator)
```

## Backup Security

### Backup Integrity

Backups are tar.gz archives with predictable structure:
- No symlinks followed (prevents symlink attacks)
- No device files included
- No setuid/setgid bits preserved

### Backup Rotation

Automatic cleanup prevents disk exhaustion:

```ini
MAX_BACKUPS=5  # Only keep 5 most recent
```

### Backup Location

Store backups in a secure location:

```ini
# Outside web-accessible directories
BACKUP_DIR=/var/backups/pluginator
```

## Logging Security

### Sensitive Data

Logs are sanitized to avoid exposing:
- API tokens
- Passwords
- Full file paths (when possible)

### Log Rotation

Automatic cleanup:

```ini
MAX_LOG_DAYS=30  # Delete logs older than 30 days
```

### Log Permissions

```bash
chmod 600 data/logs/*.log
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
