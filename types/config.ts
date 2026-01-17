/*
 * Pluginator Configuration Type Definitions
 * These types can be used for IDE autocompletion when editing your plugins.json configuration file.
 */

export type PluginSourceType = 'spigot' | 'modrinth' | 'github' | 'curseforge' | 'jenkins' | 'web';

/* Base plugin configuration shared by all source types */
export interface BasePlugin {
  name: string;             // Display name of the plugin
  type: PluginSourceType;   // Plugin source type
  version?: string;         // Current installed version (optional, auto-detected from JAR)
  enabled?: boolean;        // Whether the plugin is enabled for update checking (default: true)
  disableOnTest?: boolean;  // Disable this plugin when syncing to test server
  mcVersion?: string;       // Target Minecraft version for compatibility checking
  filenamePattern?: string; // Regex pattern for matching plugin JAR filename
}

/* Spigot plugin configuration */
export interface SpigotPlugin extends BasePlugin {
  type: 'spigot';
  resourceId: string; // Spigot resource ID (from URL: spigotmc.org/resources/name.XXXXX/)
}

/* Modrinth plugin configuration */
export interface ModrinthPlugin extends BasePlugin {
  type: 'modrinth';
  projectSlug: string; // Modrinth project slug (from URL: modrinth.com/plugin/SLUG)
}

/* GitHub plugin configuration */
export interface GitHubPlugin extends BasePlugin {
  type: 'github';
  repoSlug: string;      // GitHub repository in owner/repo format
  assetPattern?: string; // Release asset filename pattern (regex)
}

/* CurseForge plugin configuration */
export interface CurseForgePlugin extends BasePlugin {
  type: 'curseforge';
  projectId: string; // CurseForge project ID
}

/* Jenkins CI plugin configuration */
export interface JenkinsPlugin extends BasePlugin {
  type: 'jenkins';
  jenkinsUrl: string;       // Jenkins server base URL
  jobName: string;          // Jenkins job name
  artifactPattern?: string; // Artifact filename pattern (regex)
  auth?: {
    username: string; // Jenkins username
    apiToken: string; // Jenkins API token
  };
}

/* Web manifest plugin configuration */
export interface WebPlugin extends BasePlugin {
  type: 'web';
  manifestUrl: string; // URL to the update manifest JSON
}

/* Union type of all plugin configurations */
export type Plugin =
  | SpigotPlugin
  | ModrinthPlugin
  | GitHubPlugin
  | CurseForgePlugin
  | JenkinsPlugin
  | WebPlugin;

/* Main Pluginator configuration (plugins.json) */
export interface PluginatorConfig {
  plugins: Plugin[]; // Array of plugin configurations
}

/* Pluginator environment configuration (pluginator.config) */
export interface PluginatorEnvConfig {
  PLUGINATOR_DEBUG?: '0' | '1';                               // Enable debug logging
  PROD_SERVER_PATH: string;                                   // Path to production server plugins directory
  TEST_SERVER_PATH: string;                                   // Path to test server plugins directory
  BACKUP_DIR?: string;                                        // Directory for backup archives
  MAX_BACKUPS?: number;                                       // Maximum number of backups to retain
  LOGS_DIR?: string;                                          // Directory for log files
  MAX_LOG_DAYS?: number;                                      // Maximum age of log files in days
  MINECRAFT_VERSION?: string;                                 // Target Minecraft version
  API_TIMEOUT?: number;                                       // API request timeout in seconds
  DOWNLOAD_THREADS?: number;                                  // Number of concurrent download threads
  GITHUB_TOKEN?: string;                                      // GitHub personal access token (for higher rate limits)
  CURSEFORGE_API_KEY?: string;                                // CurseForge API key
  THEME?: 'default' | 'ocean' | 'forest' | 'sunset' | string; // UI theme name
}
