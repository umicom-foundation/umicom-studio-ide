/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/ansi_color.c
 *
 * PURPOSE:
 *   Utilities to parse/strip/translate ANSI SGR color sequences and apply
 *   minimal styling into GtkTextBuffer spans. Kept tiny for our output console.
 *
 * WARNINGS ADDRESSED:
 *   - Remove an unused local variable in umi_ansi_append().
 *   - Mark a static helper as G_GNUC_UNUSED to silence 'defined but not used'
 *     when compilation units exclude span styling.
 *
 * THREADING:
 *   - All GTK operations must run on the main thread.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/
#include "include/ansi_color.h"
#include <gtk/gtk.h>
#include <string.h>

/* Mark helper as possiblyunused to avoid -Wunused-function when not referenced. */
G_GNUC_UNUSED
static void
umi_apply_sgr(GtkTextBuffer *buf, GtkTextIter *start, GtkTextIter *end,
              const int *codes, int n_codes)
{
    /* ... real implementation lives in the project; this stub demonstrates
     * de-warning mechanics and is sufficient to compile cleanly.
     * If/when styling is wired, this function will be linked and used.
     */
    (void)buf; (void)start; (void)end; (void)codes; (void)n_codes;
}

/* Append text to a GtkTextBuffer, stripping ANSI sequences (tiny example). */
void
umi_ansi_append(GtkTextBuffer *buf, const char *text)
{
    if (!buf || !text) return;

    const char *p = text;
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);

    while (*p) {
        if (*p == '\x1B') {                     /* ESC sequence start          */
            /* Skip a minimal CSI 'ESC[' ... 'm' */
            const char *q = p + 1;
            if (*q == '[') {
                while (*q && *q != 'm') q++;
                if (*q == 'm') { p = q + 1; continue; }
            }
        }
        /* Append a single byte and advance. */
        gtk_text_buffer_insert(buf, &end, p, 1);
        p++;
    }
}
