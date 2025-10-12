/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/includes/privacy.h
 * PURPOSE: Public Privacy API surface (settings model + helpers)
 *
 * Design goals:
 *   - Self-contained header for the privacy module (no cross-tree includes).
 *   - Expose a small, stable C API used by llm_privacy.c and llm_http.c.
 *   - Keep ownership rules explicit and documented.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab
 * Date: 2025-10-12
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PRIVACY_H
#define UMICOM_PRIVACY_H

/* Minimal dependencies:
 * - <stdbool.h> for 'bool' in helper functions.
 * - <glib.h>    for gboolean and basic types used by the settings API.
 *
 * NOTE: We intentionally keep this header lightweight and free of other
 * project headers to avoid spaghetti dependencies.
 */
#include <stdbool.h>
#include <glib.h>

G_BEGIN_DECLS  /* Ensures C linkage when included from C++ sources */

/*-----------------------------------------------------------------------------
 * UmiPrivacySettings
 *
 * A plain-old-data struct describing privacy controls persisted to disk.
 *
 * Ownership:
 *   - The struct itself is owned by the caller (allocated by load or by you).
 *   - 'extra_redactions' is a heap string owned by the struct (g_free in free).
 *
 * Persistence:
 *   - Use umi_privacy_load() / umi_privacy_save() to read/write a JSON file.
 *---------------------------------------------------------------------------*/
typedef struct UmiPrivacySettings {
  /* Allow any network access at all (e.g., to LLM providers).                */
  gboolean allow_network;

  /* Redact absolute local file paths from prompts/logs.                       */
  gboolean redact_file_paths;

  /* Redact system/user names from prompts/logs.                               */
  gboolean redact_usernames;

  /* Disable telemetry unless a user explicitly opts in.                       */
  gboolean ban_telemetry;

  /* Optional additional redaction patterns (UTF-8, app-defined semantics).    */
  char    *extra_redactions;
} UmiPrivacySettings;

/*-----------------------------------------------------------------------------
 * Settings API (used by src/llm/llm_privacy.c)
 *
 * umi_privacy_load:
 *   Reads JSON from 'path' and returns a heap-allocated settings struct.
 *   On any failure, returns a struct pre-populated with safe defaults.
 *   Caller must free with umi_privacy_free().
 *
 * umi_privacy_save:
 *   Serializes 's' to JSON and writes it to 'path'.
 *   Returns TRUE on success; FALSE on IO/serialization error.
 *
 * umi_privacy_free:
 *   Frees any owned strings within the struct and then the struct itself.
 *   Safe to pass NULL (no-op).
 *---------------------------------------------------------------------------*/
UmiPrivacySettings *umi_privacy_load(const char *path);
gboolean            umi_privacy_save(const char *path, const UmiPrivacySettings *s);
void                umi_privacy_free(UmiPrivacySettings *s);

/*-----------------------------------------------------------------------------
 * Lightweight runtime helpers (used by src/llm/llm_http.c)
 *
 * umi_privacy_is_local_only:
 *   Returns true if “local-only” mode is enabled (e.g., via environment).
 *
 * umi_privacy_allow_url:
 *   Validates whether a given absolute URL is allowed under the current
 *   policy. In local-only mode, permits only file:// and loopback hosts.
 *   On denial, writes a short reason string into 'errbuf' if provided.
 *
 * Parameters:
 *   - url    : absolute URL (MUST NOT be NULL)
 *   - errbuf : optional buffer for error reason (may be NULL)
 *   - errcap : capacity of errbuf, including the trailing '\0'
 *
 * Returns:
 *   true  if allowed
 *   false if denied (and 'errbuf' contains a reason when provided)
 *---------------------------------------------------------------------------*/
bool umi_privacy_is_local_only(void);
bool umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap);

G_END_DECLS

#endif /* UMICOM_PRIVACY_H */
/*---------------------------------------------------------------------------*/
/*--------------------------------- End of file --------------------------------*/