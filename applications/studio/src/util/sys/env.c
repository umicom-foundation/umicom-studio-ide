/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/env.c
 *
 * PURPOSE:
 *   Small helpers around environment variables (get/set with defaults).
 *
 * API:
 *   gchar   *umi_env_get(const char *key);                 // g_free()
 *   gchar   *umi_env_get_or(const char *key, const char *fallback); // g_free()
 *   gboolean umi_env_set(const char *key, const char *value, gboolean overwrite);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <env.h>

gchar *umi_env_get(const char *key) {
    if (!key) return NULL;
    const char *v = g_getenv(key);
    return v ? g_strdup(v) : NULL;
}

gchar *umi_env_get_or(const char *key, const char *fallback) {
    if (!key) return fallback ? g_strdup(fallback) : NULL;
    const char *v = g_getenv(key);
    return g_strdup(v ? v : fallback);
}

gboolean umi_env_set(const char *key, const char *value, gboolean overwrite) {
    if (!key) return FALSE;
    return g_setenv(key, value ? value : "", overwrite);
}
