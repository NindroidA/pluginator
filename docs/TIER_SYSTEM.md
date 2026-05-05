# Pluginator Tier System

> **Last Updated:** April 30, 2026

## Overview

Pluginator uses a tiered subscription system to support ongoing development while keeping the core experience available to everyone. The free tier is intentionally narrow — it's enough to try the workflow on a single server, but daily plugin management really starts at Plus.

## Tiers

| Tier | Price | Update Checks/Day | Downloads/Day | Syncs/Day | Migrations/Day |
|------|-------|-------------------|---------------|-----------|----------------|
| **Free** | $0 | 1 | 5 | 1 | 0 |
| **Plus** | $9.99 (one-time) | 10 | 50 | 5 | 3 |
| **Pro** | $2.99/mo or $24.99/yr | 25 | 150 | 15 | 10 |
| **Max** | $4.99/mo or $44.99/yr | Unlimited | Unlimited | Unlimited | Unlimited |

> **Note:** "Migrations" means promoting plugins from your test server to production. The free tier does not include any migrations per day — Plus is the entry point for the test → prod workflow.

### Plus Discount

Plus tier holders are existing supporters and receive a permanent discount on every subscription tier:

| Subscription | Standard | With Plus Discount |
|--------------|----------|--------------------|
| Pro Monthly | $2.99/mo | **$1.99/mo** |
| Pro Annual | $24.99/yr | **$19.99/yr** |
| Max Monthly | $4.99/mo | **$3.99/mo** |
| Max Annual | $44.99/yr | **$39.99/yr** |

The discount is applied automatically — once you own Plus, every checkout for Pro or Max uses the discounted price.

## Features by Tier

### Free Tier

- Core workflow at low volume: 1 update check, 5 downloads, 1 sync per day
- Home, Plugins, and Updates tabs
- Curated plugin registry (104 plugins)
- All 8 plugin sources (Spigot, Modrinth, GitHub, Bukkit, CurseForge, Hangar, Jenkins, GeyserMC, Web Manifest)
- Setup wizard, backup/restore, configuration editing

### Plus Tier ($9.99 one-time)

Everything in Free, plus:

- **Servers tab** — side-by-side production vs test comparison
- **Logs tab** — real-time application log viewer
- **Stats tab** — plugin statistics, source breakdowns, version distribution
- Higher daily limits (10 checks, 50 downloads, 5 syncs, **3 migrations**)
- Tags and Groups for plugin organization
- Jump-list navigation (`Ctrl+O` / `Ctrl+I`, vim-style)
- Chord key sequences (Space-leader shortcuts)
- Dashboard widget customization
- Permanent discount on Pro/Max subscriptions

### Pro Tier ($2.99/mo or $24.99/yr)

Everything in Plus, plus:

- **Health Dashboard** — per-plugin health scores, security advisories, compatibility checks
- **Recommendations** — smart plugin suggestions based on your setup
- **Performance Analytics** — TPS, MSPT, memory, and player metrics with panel integrations
- Higher daily limits (25 checks, 150 downloads, 15 syncs, 10 migrations)
- Plugin Profiles (save and apply named configurations)
- Dependency Graph viewer
- Update Schedule (automatic background daemon)
- Macros (record/play key sequences)
- Export/Import for plugins, servers, tags, and groups
- Vim mode

### Max Tier ($4.99/mo or $44.99/yr)

Everything in Pro, plus:

- **Unlimited** daily limits across every action
- **Theme Marketplace** — browse and install community themes
- **Accessibility Toggle** (sticky keys, slow keys, screen-reader hints)
- Early access to new features
- Direct developer support

## Authentication

### Email/Password Login

```bash
# CLI subcommand
pluginator login

# In the UI
/login
```

### OAuth (Google/GitHub)

1. Run `/login` (or `pluginator login`)
2. Choose Google or GitHub
3. A browser window opens for authentication
4. Pluginator returns to the CLI automatically once the OAuth flow completes

OAuth uses a local callback server on port 9876. If the port is in use, close other Pluginator instances first.

### Session Storage

Sessions are stored encrypted at `~/.pluginator/session.json`:

- AES-256-GCM encryption
- Machine-specific encryption key (derived from a per-install salt at `~/.pluginator/.salt`)
- 24-hour expiry with automatic refresh on use

## Account Commands

| Command | Description |
|---------|-------------|
| `/login` | Login to your Pluginator account |
| `/logout` | Logout from your account |
| `/account` | View account, tier, and today's usage |
| `/upgrade` | Open the upgrade flow in your browser |

## Usage Tracking

Daily usage resets at midnight UTC. View current usage with `/account`:

```
Account: user@example.com
Tier: Plus

Today's Usage:
  Update Checks: 4/10
  Downloads:     12/50
  Syncs:         2/5
  Migrations:    0/3
```

## Upgrading

When you hit a daily limit, Pluginator shows an upgrade prompt with the next tier's pricing. Upgrades are processed through the web portal at https://nindroidsystems.com/pluginator. After purchase, run `/account` to refresh your tier — sessions update automatically on the next API call.

## Offline Behavior

When offline, Pluginator:

- Uses cached tier information from the last successful sign-in
- Allows operations within your cached daily limits
- Syncs usage counters back to the server when connectivity returns

## Environment Variables

```bash
# Custom API endpoint (development / self-hosted)
PLUGINATOR_API_URL=https://api.nindroidsystems.com
```

## Troubleshooting

### "Session expired"

Your 24-hour session has expired. Run `/login` to authenticate again.

### "Usage limit reached"

You've hit your daily limit. Options:

1. Wait until midnight UTC for the daily reset
2. Upgrade your tier for higher (or unlimited) limits

### OAuth doesn't return to the CLI

If the browser stays open after OAuth:

1. Check that port 9876 is available (`lsof -i :9876`)
2. Close any other running Pluginator instances
3. Close the browser tab manually and try `/login` again

### Session not persisting between runs

Encrypted sessions require:

- Write access to `~/.pluginator/`
- A consistent machine ID (hostname and user)

If you're running Pluginator inside containers or VMs that regenerate machine IDs, the session may need to be recreated each run.

## See Also

- [USER_GUIDE.md](USER_GUIDE.md) — General usage
- [COMMANDS.md](COMMANDS.md) — Full command reference with tier requirements
