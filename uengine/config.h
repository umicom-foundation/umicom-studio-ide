/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: include/ueng/config.h
 * PURPOSE: Tiny, dependency-free configuration API for uaengine.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 25-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*
 * OVERVIEW
 * -------
 * This header declares a minimal "config" facility that:
 *   - Loads defaults into a UengConfig struct (ueng_config_defaults).
 *   - Optionally merges a simple "key: value" config file (ueng_config_load_file).
 *   - Optionally applies environment variable overrides (ueng_config_apply_env).
 *   - Optionally exports a few environment variables if they weren't already
 *     set (ueng_config_export_env). This is convenient to keep the rest of
 *     the codebase unchanged because existing LLM provider selection reads
 *     env vars like UENG_LLM_PROVIDER and UENG_LLM_MODEL.
 *
 * The parser intentionally supports a flat subset of YAML:
 *   - Lines like:   key: value
 *   - Comments start with '#' anywhere on the line
 *   - Leading/trailing spaces are ignored
 *   - Empty lines are ignored
 * This keeps it extremely portable and dependency-free.
 *
 * IMPORTANT: This facility is **additive**. You don't have to call it if you
 * don't need config files. Existing behavior using direct env vars remains intact.
 *---------------------------------------------------------------------------*/
#ifndef UENG_CONFIG_H
#define UENG_CONFIG_H

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C"
{
#endif

  /* A tiny POD structure that holds the most common knobs we use today.
   * NOTE: These are only defaults + "last loaded" values. They do not
   * magically affect your program until you either read them or export
   * them via ueng_config_export_env(). */
  typedef struct UengConfig
  {
    /* LLM selection (one of: "openai", "ollama", "llama"). */
    char llm_provider[32]; /* e.g., "openai" (case-sensitive) */
    char llm_model[128];   /* e.g., "gpt-4o-mini", "qwen2.5:3b", "model.gguf" */

    /* OpenAI (cloud) */
    char openai_api_key[256];  /* usually provided via env instead of file */
    char openai_base_url[256]; /* optional custom endpoint */

    /* Ollama (local server) */
    char ollama_host[128]; /* e.g., "http://127.0.0.1:11434" */

    /* llama.cpp (fully in-process) */
    char llama_model_path[256]; /* path to .gguf model (if embedding llama.cpp) */

    /* Dev server knobs (used by 'serve') */
    char serve_host[64]; /* e.g., "127.0.0.1" */
    int serve_port;      /* e.g., 8080 */

    /* Project paths (nice-to-have defaults) */
    char workspace_dir[256]; /* e.g., "workspace" */
    char site_root[256];     /* e.g., "site" */
  } UengConfig;

  /* Initialize with reasonable defaults. Safe to call on any stack/zeroed struct. */
  void ueng_config_defaults(UengConfig *c);

  /* Merge a simple "key: value" config file into 'c'.
   * RETURNS: 0 on success; non-zero if the file could not be read (format errors
   * are tolerated by ignoring unknown keys). */
  int ueng_config_load_file(UengConfig *c, const char *path);

  /* Apply environment variable overrides (non-empty values win).
   * Env vars examined (all optional):
   *   - UENG_LLM_PROVIDER, UENG_LLM_MODEL
   *   - OPENAI_API_KEY, UENG_OPENAI_BASE_URL
   *   - UENG_OLLAMA_HOST
   *   - UENG_LLAMA_MODEL_PATH
   *   - UENG_SERVE_HOST, UENG_SERVE_PORT
   *   - UENG_WORKSPACE_DIR, UENG_SITE_ROOT */
  void ueng_config_apply_env(UengConfig *c);

  /* Export the most common env vars if they are not already defined or if
   * 'overwrite' is non-zero. This lets legacy code that reads env vars continue
   * to work without any further changes. */
  void ueng_config_export_env(const UengConfig *c, int overwrite);

  /* Convenience helper: load defaults, then file (if present), then env.
   * RETURNS: 0 (even if file missing) to keep it non-fatal; >0 if critical
   * error (currently always 0). */
  int ueng_config_init_from(const char *file_or_null, UengConfig *out);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UENG_CONFIG_H */
