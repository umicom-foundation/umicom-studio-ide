/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/utf8.c
 * PURPOSE: UTF-8 helpers (case-insensitive contains)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>   /* g_utf8_* helpers, g_strstrip, g_ascii_tolower */

#include <utf8.h>

/* Case-insensitive “contains” for UTF-8 strings using GLib case folding.      */
/* Returns TRUE if haystack contains needle (empty needle is TRUE by convention). */
gboolean umi_utf8_icontains(const char *haystack, const char *needle) {
    if (!haystack || !needle) return FALSE;  /* strict: NULL is not a match     */
    if (*needle == '\0') return TRUE;        /* empty needle always matches     */

    /* Use casefolding to approximate case-insensitive matching for Unicode.    */
    g_autofree gchar *h = g_utf8_casefold(haystack, -1);
    g_autofree gchar *n = g_utf8_casefold(needle,   -1);

    /* Then use g_strstr_len on the case-folded strings (binary safe with len). */
    return g_strstr_len(h, -1, n) != NULL;
}
/* --- IGNORE --- */