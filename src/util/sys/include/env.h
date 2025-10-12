/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/env.h
 * PURPOSE:
 *   Public, GLib-friendly helpers for getting/setting environment variables.
 *
 * DESIGN:
 *   - Small, self-contained API used by multiple util/sys modules (e.g. msys_env.c).
 *   - Returns heap-allocated GLib strings for convenience; callers free with g_free().
 *   - Mirrors the implementations in src/util/sys/env.c exactly (no behavior change).
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ENV_H
#define UMICOM_ENV_H

#include <glib.h>   /* gchar, gboolean */

/*-----------------------------------------------------------------------------
 * umi_env_get
 *
 * PURPOSE:
 *   Fetch the value of an environment variable as a newly allocated string.
*   Tiny, compile-time driven helpers to branch platform-specific behavior
 *   at runtime without sprinkling #ifdefs through higher-level code.
 *
 * PARAMS:
 *   key  - Environment variable name (UTF-8).
 *
 * RETURNS:
 *   Newly allocated string containing the value (use g_free), or NULL if unset.
 *---------------------------------------------------------------------------*/
gchar *umi_env_get(const char *key);

/*-----------------------------------------------------------------------------
 * umi_env_get_or
 *
 * PURPOSE:
 *   Fetch the value of an environment variable, or return a duplicate of the
 *   provided fallback if the variable is missing or empty.
 *
 * PARAMS:
 *   key      - Environment variable name (UTF-8).
 *   fallback - Fallback string used when variable is not set or empty.
 *
 * RETURNS:
 *   Newly allocated string (use g_free). Never returns NULL; returns a duplicate
 *   of 'fallback' (or an empty string if fallback is NULL).
 *---------------------------------------------------------------------------*/
gchar *umi_env_get_or(const char *key, const char *fallback);

/*-----------------------------------------------------------------------------
 * umi_env_set
 *
 * PURPOSE:
 *   Set (or unset) an environment variable.
 *
 * PARAMS:
 *   key       - Environment variable name.
 *   value     - New value; if NULL, the variable is unset.
 *   overwrite - When FALSE and the variable already exists, no change is made.
 *
 * RETURNS:
 *   TRUE on success, FALSE on failure.
 *---------------------------------------------------------------------------*/
gboolean umi_env_set(const char *key, const char *value, gboolean overwrite);

#endif /* UMICOM_ENV_H */
/*--- end of file ---*/