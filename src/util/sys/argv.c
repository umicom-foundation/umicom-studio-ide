/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/argv.c
 * PURPOSE: Cross-platform argv utilities (Windows quoting, join/split helpers)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>     /* GPtrArray, g_autofree, g_str* helpers */
#include <string.h>   /* strlen, strchr                         */
#include <stdlib.h>   /* malloc/free (via glib as well)         */
#include <stdio.h>    /* snprintf                               */

#include <argv.h>     /* Project header from src/util/sys/include */

/* Return TRUE if a character requires quoting in Windows command-line rules. */
/* Reference: cmd.exe / CreateProcess quoting quirks (spaces, tabs, quotes).  */
static gboolean umi_win_needs_quote_char(char c) {
    return (c == ' ' || c == '\t' || c == '"' || c == '\n' || c == '\r');
}

/* Quote one argument for Windows per CreateProcess rules.                       */
/* We double backslashes that precede a quote and wrap the whole arg in quotes   */
/* when necessary. This mirrors common implementations (e.g., GLib g_shell_*).  */
static gchar *umi_win_quote_arg(const char *arg) {
    if (!arg || !*arg) {
        /* Empty arg becomes "" */
        return g_strdup("\"\"");
    }

    gboolean needs_quote = FALSE;      /* track if we must wrap in quotes */
    for (const char *p = arg; *p; ++p) {
        if (umi_win_needs_quote_char(*p)) { needs_quote = TRUE; break; }
    }

    if (!needs_quote) {
        /* No special chars → return verbatim. */
        return g_strdup(arg);
    }

    /* Worst case: every char might need escaping; reserve 2x + 2 for quotes. */
    GString *s = g_string_sized_new((gssize)(strlen(arg) * 2 + 2));

    g_string_append_c(s, '"'); /* opening quote */

    /* Walk through and escape as required. */
    int bs_count = 0;  /* number of consecutive backslashes seen */
    for (const char *p = arg; *p; ++p) {
        if (*p == '\\') {
            ++bs_count;               /* count backslashes to handle " cases */
        } else if (*p == '"') {
            /* A quote is preceded by N backslashes -> emit 2N+1 backslashes. */
            for (int i = 0; i < bs_count * 2 + 1; ++i) g_string_append_c(s, '\\');
            g_string_append_c(s, '"'); /* copy the quote itself */
            bs_count = 0;              /* reset */
        } else {
            /* Any other char: first flush the backslashes seen so far. */
            for (int i = 0; i < bs_count; ++i) g_string_append_c(s, '\\');
            bs_count = 0;
            g_string_append_c(s, *p); /* copy the normal char */
        }
    }

    /* At end: if arg ended with backslashes, double them before the closing quote. */
    for (int i = 0; i < bs_count * 2; ++i) g_string_append_c(s, '\\');
    g_string_append_c(s, '"'); /* closing quote */

    return g_string_free(s, FALSE); /* transfer str, free GString */
}

/* Join a NULL-terminated argv vector into a single string. */
/* On Windows, we apply Windows quoting; on other OS, we join with spaces.     */
gchar *umi_argv_join(char *const argv[]) {
    if (!argv) return g_strdup("");

    GString *out = g_string_sized_new(128); /* growable result */

#ifdef G_OS_WIN32
    /* Apply Windows quoting rules per arg. */
    for (int i = 0; argv[i]; ++i) {
        g_autofree gchar *q = umi_win_quote_arg(argv[i]); /* quoted form */
        if (i) g_string_append_c(out, ' ');               /* separator   */
        g_string_append(out, q);                          /* append      */
    }
#else
    /* POSIX shells typically don’t need special handling for display purposes. */
    for (int i = 0; argv[i]; ++i) {
        if (i) g_string_append_c(out, ' ');
        g_string_append(out, argv[i]);
    }
#endif

    return g_string_free(out, FALSE); /* caller takes ownership */
}

/* Split a command line using GLib’s shell semantics on POSIX and Windows-safe
 * semantics on Windows. Returns a newly allocated argv vector (NULL-terminated).
 * Caller must free with g_strfreev().                                          */
gchar **umi_argv_split(const char *line) {
    if (!line) return g_new0(char*, 1); /* return empty argv */

#ifdef G_OS_WIN32
    /* On Windows, we approximate splitting by scanning quotes and spaces. */
    GPtrArray *args = g_ptr_array_new_with_free_func(g_free);
    const char *p = line;
    while (*p) {
        /* Skip leading spaces */
        while (*p == ' ' || *p == '\t') ++p;

        if (!*p) break;

        gboolean in_quotes = FALSE;
        GString *arg = g_string_sized_new(16);
        int bs = 0; /* consecutive backslashes */

        while (*p) {
            char c = *p++;
            if (c == '\\') {
                ++bs; /* count backslashes */
                continue;
            }
            if (c == '"') {
                /* A quote preceded by N backslashes => emit N/2 backslashes,
                 * and if N is even, toggle quoting; if odd, it’s a literal quote. */
                for (int i = 0; i < bs / 2; ++i) g_string_append_c(arg, '\\');
                if ((bs % 2) == 0) {
                    in_quotes = !in_quotes; /* toggle quote mode */
                } else {
                    g_string_append_c(arg, '"'); /* literal quote */
                }
                bs = 0;
                continue;
            }
            /* Flush backslashes before normal chars or spaces. */
            for (int i = 0; i < bs; ++i) g_string_append_c(arg, '\\');
            bs = 0;

            if (!in_quotes && (c == ' ' || c == '\t')) {
                break; /* end of this argument */
            }
            g_string_append_c(arg, c);
        }

        g_ptr_array_add(args, g_string_free(arg, FALSE)); /* push arg string */
        /* Continue scanning (p already at next char or space). */
    }
    g_ptr_array_add(args, NULL); /* NULL-terminate */
    return (gchar **)g_ptr_array_free(args, FALSE);
#else
    /* POSIX: use GLib helper for shell-style splitting. */
    g_autoptr(GError) err = NULL;
    gchar **v = NULL;
    if (!g_shell_parse_argv(line, NULL, &v, &err)) {
        /* On parse error, return an empty vector to be safe. */
        (void)err; /* keeping the variable for debugging; not printed here */
        return g_new0(char*, 1);
    }
    return v; /* GLib-allocated vector, free with g_strfreev */
#endif
}
/* --- IGNORE --- */