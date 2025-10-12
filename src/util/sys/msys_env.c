/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/msys_env.c
 * PURPOSE: MSYS2/MinGW compatibility helpers (derive PATH and prefixes)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

/* existing headers... */
#include <glib.h>
#include <gio/gio.h>
#include "msys_env.h"
#include "env.h"          /* <-- add this line: public prototypes for umi_env_* */




/* Return TRUE if this process looks like it’s running under MSYS/MinGW shells. */
/* We check a couple of common environment markers.                              */
gboolean umi_msys_detect(void) {
#ifdef G_OS_WIN32
    const char *msystem = g_getenv("MSYSTEM");  /* MSYS shells export this */
    const char *mingw   = g_getenv("MINGW_PREFIX");
    return (msystem && *msystem) || (mingw && *mingw);
#else
    return FALSE; /* Non-Windows → not MSYS */
#endif
}

/* Return a best-effort PATH for running external tools on MSYS/MinGW.           */
/* This function is intentionally conservative: it starts from the existing PATH */
/* and may add a few common MSYS locations if they are missing.                  */
gchar *umi_msys_best_path(void) {
#ifndef G_OS_WIN32
    /* Non-Windows: just copy PATH or empty. */
    const char *p = g_getenv("PATH");
    return g_strdup(p ? p : "");
#else
    /* Windows: If under MSYS, include the MSYS usr/bin in front for consistency. */
    g_autofree gchar *cur = umi_env_get("PATH");  /* copy current PATH (may be NULL) */
    const char *prefix = g_getenv("MSYS2_PREFIX");/* some installations expose this  */
    const char *usrbin = prefix ? "\\usr\\bin" : NULL;

    if (prefix && usrbin) {
        /* Build a candidate: <prefix>\usr\bin;<existing PATH> */
        GString *out = g_string_new(prefix);
        g_string_append(out, usrbin);
        if (cur && *cur) {
            g_string_append_c(out, ';');
            g_string_append(out, cur);
        }
        return g_string_free(out, FALSE);
    }

    /* Fallback: return the current PATH as-is. */
    return g_strdup(cur ? cur : "");
#endif
}
/* Return a semicolon-separated PATH prefix for MSYS/MinGW tools, or NULL.      */