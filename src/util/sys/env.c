/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/env.c
 * PURPOSE: Small helpers around environment variables (get/set with defaults)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>  /* g_getenv, g_setenv, g_strdup */

#include <env.h>   /* public header */

/* Get an environment variable; returns a newly allocated copy or NULL.     */
/* Caller must free the returned string with g_free().                      */
gchar *umi_env_get(const char *key) {
    if (!key) return NULL;                      /* defensive: NULL key       */
    const char *v = g_getenv(key);              /* read process environment  */
    return v ? g_strdup(v) : NULL;              /* copy or NULL if not set   */
}

/* Get an environment variable with a default if missing.                   */
/* Returns a newly allocated string: either value or default (copied).      */
gchar *umi_env_get_or(const char *key, const char *fallback) {
    if (!key) return fallback ? g_strdup(fallback) : NULL; /* defensive */
    const char *v = g_getenv(key);                          /* env value */
    return g_strdup(v ? v : fallback);                      /* duplicate  */
}

/* Set an environment variable; overwrite controls whether to replace.      */
/* Returns TRUE on success.                                                 */
gboolean umi_env_set(const char *key, const char *value, gboolean overwrite) {
    if (!key) return FALSE;                         /* invalid key             */
    return g_setenv(key, value ? value : "", overwrite); /* GLib helper       */
}
