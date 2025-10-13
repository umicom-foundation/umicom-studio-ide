/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/utf8.c
 *
 * PURPOSE:
 *   UTF-8 helpers (case-insensitive substring match) using GLib case folding.
 *
 * API:
 *   gboolean umi_utf8_icontains(const char *haystack, const char *needle);
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <utf8.h>

gboolean umi_utf8_icontains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return FALSE;
    if (*needle == '\0') return TRUE;

    g_autofree gchar *h = g_utf8_casefold(haystack, -1);
    g_autofree gchar *n = g_utf8_casefold(needle,   -1);
    return g_strstr_len(h, -1, n) != NULL;
}
