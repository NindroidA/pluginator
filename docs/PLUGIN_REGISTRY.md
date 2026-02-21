# Plugin Registry System

> **Last Updated:** February 21, 2026

## Overview

The plugin registry provides quick access to 100+ popular Minecraft plugins with pre-configured source identifiers. Instead of manually entering Spigot resource IDs or Modrinth slugs, you can simply search for a plugin by name. The full registry is served via the NinSys API, with 41 plugins bundled locally as a fallback.

## Curated Registry (Bundled)

The bundled registry at `data/defaults/plugin-registry.json` contains pre-configured popular plugins with verified source identifiers.

### Included Plugins

| Plugin | Category | Sources |
|--------|----------|---------|
| EssentialsX | utility | Modrinth, Spigot, GitHub |
| LuckPerms | permissions | Modrinth, Spigot, GitHub |
| Vault | economy | Spigot, GitHub |
| WorldEdit | world | Modrinth, Spigot |
| WorldGuard | protection | Modrinth, Spigot |
| FAWE | world | Modrinth |
| CoreProtect | admin | Modrinth, Spigot |
| PlaceholderAPI | library | Spigot, GitHub |
| ProtocolLib | library | Spigot, GitHub |
| Citizens | gameplay | Spigot |
| spark | performance | Modrinth, Spigot |
| DiscordSRV | social | Spigot, GitHub |
| TAB | cosmetic | Modrinth, Spigot |
| Multiverse-Core | world | Modrinth, Spigot |
| mcMMO | gameplay | Spigot |
| Jobs Reborn | economy | Spigot |
| Towny | gameplay | Spigot, GitHub |
| ViaVersion | utility | Modrinth, Spigot, GitHub |
| Chunky | utility | Modrinth, Spigot |
| Geyser | utility | Modrinth |
| *...and 80+ more* | | |

## Custom Registry (User)

Add your own plugins to `~/.pluginator/custom-registry.json`:

```json
{
  "plugins": [
    {
      "id": "my-plugin",
      "name": "My Plugin",
      "description": "A custom plugin I use",
      "category": "utility",
      "authors": ["AuthorName"],
      "tags": ["custom", "private"],
      "sources": [
        {
          "type": "github",
          "identifier": "myuser/myplugin",
          "preferred": true
        }
      ]
    }
  ]
}
```

User plugins override bundled plugins with the same ID.

## Using the Registry

### Adding a Plugin from Registry

1. Open the command palette (`:` or `/`)
2. Type `/add` or navigate to Add Plugin
3. Search for the plugin by name
4. Select from search results
5. Plugin is added with optimal source configuration

### Searching the Registry

```
Search: luck

Results:
1. LuckPerms (permissions) - Confidence: 100%
2. LuckPerms Extension (utility) - Confidence: 65%
```

The search uses fuzzy matching with confidence scoring:
- Exact ID match: 100%
- Exact name match: 95%
- Name starts with: 80%
- Name contains: 60%
- Tag match: 40%
- Description match: 30%

### Viewing Plugin Details

Select a plugin to see:
- Name and description
- Authors
- Available sources (with preferred marked)
- Category and tags
- Minecraft version compatibility
- Dependencies and conflicts

## Registry Format

### Plugin Entry Schema

```typescript
interface RegistryPlugin {
  id: string;           // Unique identifier (lowercase, hyphenated)
  name: string;         // Display name
  description: string;  // Brief description
  category: PluginCategory;
  authors: string[];
  tags: string[];

  sources: Array<{
    type: SourceType;   // 'modrinth' | 'spigot' | 'github' | etc.
    identifier: string; // Source-specific ID/slug
    preferred?: boolean;
  }>;

  // Optional fields
  mcVersions?: {
    min?: string;       // e.g., "1.16"
    max?: string;       // e.g., "1.21"
  };
  dependencies?: string[];  // Plugin IDs
  conflicts?: string[];     // Plugin IDs
  popular?: boolean;
  verified?: boolean;
}
```

### Categories

| Category | Description |
|----------|-------------|
| `admin` | Server administration tools |
| `economy` | Economy and currency systems |
| `permissions` | Permission management |
| `world` | World editing and management |
| `gameplay` | Gameplay modifications |
| `chat` | Chat systems and formatting |
| `protection` | Land/griefing protection |
| `utility` | General utilities |
| `library` | Developer libraries |
| `performance` | Performance optimization |
| `cosmetic` | Visual enhancements |
| `minigames` | Minigame plugins |
| `social` | Social features (Discord, etc.) |

### Source Types

| Type | Identifier Format | Example |
|------|-------------------|---------|
| `modrinth` | Project slug | `luckperms` |
| `spigot` | Resource ID | `28140` |
| `hangar` | Project slug | `LuckPerms` |
| `github` | owner/repo | `LuckPerms/LuckPerms` |
| `curseforge` | Project ID | `12345` |
| `jenkins` | job path | `LuckPerms/job/main` |

## Preferred Source Selection

Each plugin can have multiple sources. The preferred source is selected by:

1. Explicit `preferred: true` flag
2. First source in the list (if no preferred set)

Preferred sources are used for:
- Update checking
- Downloads
- Version comparison

## Programmatic Access

```typescript
import { getRegistryService } from './core/registry/index.js';

const registry = getRegistryService();
registry.load();

// Search for plugins
const results = registry.search('permissions');

// Get by ID
const luckperms = registry.getById('luckperms');

// Get popular plugins
const popular = registry.getPopular(10);

// Get by category
const adminPlugins = registry.getByCategory('admin');

// Get preferred source for a plugin
const source = registry.getPreferredSource(luckperms);
```

## Contributing to the Registry

To add a plugin to the curated registry:

1. Fork the Pluginator repository
2. Edit `data/defaults/plugin-registry.json`
3. Add your plugin entry following the schema
4. Run `bun run sort-registry` to maintain alphabetical order
5. Submit a pull request

### Requirements for Inclusion

- Plugin must be publicly available
- At least one verified source
- Accurate metadata (description, authors, tags)
- Active maintenance preferred
