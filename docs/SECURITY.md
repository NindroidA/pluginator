# Security Policy

> **Last Updated:** January 16, 2025

## Security Model

Pluginator operates with the following security principles:

1. **Least Privilege** - Only requests permissions needed for operation
2. **Defense in Depth** - Multiple layers of validation and protection
3. **Fail Secure** - Errors default to safe/conservative behavior
4. **No Remote Code Execution** - Never evaluates arbitrary code

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.5.x   | :white_check_mark: |
| < 1.5   | :x:                |

## Reporting a Vulnerability

**Do not report security vulnerabilities publicly.**

### How to Report

1. **Preferred:** [Open a private security advisory](https://github.com/NindroidA/pluginator/security/advisories/new)
2. **Alternative:** Contact via [GitHub profile](https://github.com/NindroidA)

### What to Include

1. Description of the vulnerability
2. Steps to reproduce
3. Potential impact
4. Suggested fix (if any)

### Response Timeline

- **Acknowledgment:** 48 hours
- **Assessment:** 7 days
- **Fix (if confirmed):** 30 days

## Security Features

### Path Traversal Prevention

All file operations use safe path utilities that prevent directory traversal attacks.

### Filename Sanitization

Downloaded files have their names sanitized to remove:
- Path separators (`/`, `\`)
- Special characters (`<`, `>`, `:`, `"`, `|`, `?`, `*`)
- Control characters
- Leading dots

### Checksum Verification

Downloads can be verified against checksums (SHA-256, SHA-512, SHA-1, MD5).

### HTTPS Enforcement

All API communications use HTTPS.

### Credential Management

API credentials should be stored securely:
- Use environment variables (preferred)
- Or config files with restricted permissions (`chmod 600`)

**Never commit credentials to version control.**

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
