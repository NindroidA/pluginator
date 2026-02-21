# Pluginator Tier System

> **Last Updated:** February 21, 2026

## Overview

Pluginator uses a tiered subscription system to support ongoing development while keeping the core features free.

## Tiers

| Tier | Price | Checks/Day | Downloads/Day | Syncs/Day | Max Servers | Custom Servers |
|------|-------|------------|---------------|-----------|-------------|----------------|
| **Free** | $0 | 10 | 5 | 3 | 1 | No |
| **Plus** | $14.99 (one-time) | 50 | 25 | 15 | 3 | No |
| **Pro** | $4.99/mo | 100 | 50 | 30 | 10 | Yes |
| **Max** | $9.99/mo | Unlimited | Unlimited | Unlimited | Unlimited | Yes |

### Plus Discount

Plus tier holders receive permanent discounts on subscription tiers:
- **40% off Pro** ($2.99/mo instead of $4.99/mo)
- **20% off Max** ($7.99/mo instead of $9.99/mo)

## Features by Tier

### Free Tier
- Basic update checking (10/day)
- Basic downloads (5/day)
- Basic syncs (3/day)
- 1 server
- Access to curated plugin registry
- Core UI tabs (Home, Servers, Plugins, Updates, Logs)

### Plus Tier
- Everything in Free
- Increased limits (50 checks, 25 downloads, 15 syncs)
- Up to 3 servers
- **Stats Dashboard** tab
- **Tags & Groups** for plugin organization
- Chord key sequences
- Permanent discount on Pro/Max
- One-time purchase (no subscription)

### Pro Tier
- Everything in Plus
- Higher limits (100 checks, 50 downloads, 30 syncs)
- Up to 10 servers, 25 GB cloud storage
- Custom server configurations
- **Health Dashboard** tab
- **Recommendations** tab
- **Performance Analytics** tab
- Dependency graph, plugin profiles
- Update schedule, macros, export/import
- Priority support

### Max Tier
- Unlimited everything
- All Pro features
- 50 GB cloud storage, API access
- **Theme Marketplace**
- **Accessibility Settings**
- Early access to new features
- Direct developer support

## Authentication

### Email/Password Login

```bash
# In the UI
/login

# Or via keyboard shortcut
Press 'l' in the UI
```

### OAuth (Google/GitHub)

1. Run `/login` command
2. Select Google or GitHub
3. Browser opens for authentication
4. Automatically returns to CLI after success

### Session Storage

Sessions are stored encrypted at `~/.pluginator/session.json`:
- AES-256-GCM encryption
- Machine-specific encryption key
- 24-hour expiry
- Automatic refresh on use

## Commands

| Command | Description |
|---------|-------------|
| `/login` | Login to your account |
| `/logout` | Logout from your account |
| `/account` | View account and usage info |

## Usage Tracking

Your daily usage resets at midnight UTC. View current usage with `/account`:

```
Account: user@example.com
Tier: Plus

Today's Usage:
  Update Checks: 12/50
  Downloads: 3/25
  Syncs: 1/15
```

## Upgrading

When you hit a limit, Pluginator shows an upgrade prompt with pricing information. Upgrades are handled through the web portal at https://nindroidsystems.com/pluginator.

## Offline Usage

When offline, Pluginator:
- Uses cached tier information
- Allows operations within cached limits
- Syncs usage when back online

## Environment Variables

```bash
# Custom API endpoint (for development)
PLUGINATOR_API_URL=https://api.nindroidsystems.com
```

## Troubleshooting

### "Session expired" error

Your 24-hour session has expired. Run `/login` to authenticate again.

### "Usage limit reached" error

You've hit your daily limit. Options:
1. Wait until midnight UTC for reset
2. Upgrade your tier for higher limits

### OAuth not returning to CLI

If the browser doesn't return to CLI after OAuth:
1. Check if port 9876 is available
2. Try closing other Pluginator instances
3. Manually close the browser tab and try again

### Session not persisting

Sessions require:
- Write access to `~/.pluginator/`
- Consistent machine ID (hostname, user)

If using containers or VMs, the machine ID may change between runs.
