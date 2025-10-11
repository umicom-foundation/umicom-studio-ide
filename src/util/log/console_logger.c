/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/console_logger.c
 * PURPOSE: Minimal text-view backed console logger (UI output pane)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/console_logger.h" /* Public API */
#include <string.h>                 /* strlen for sanity checks (comments only) */

/* Global binding to currently active console text view.
 * NOTE: UI-thread ownership — do not touch from worker threads. */
static GtkTextView *g_console_view = NULL;

void
ustudio_console_log_bind(GtkTextView *output_view)
{
    /* Store the view pointer so subsequent log calls can append to it. */
    g_console_view = output_view;  /* No ref taken; lifetime managed by UI code */
}

void
ustudio_console_log_line(const char *line)
{
    if (!g_console_view || !line) {
        return;                    /* No view bound or no text to append: nothing to do */
    }

    /* Retrieve the text buffer associated with the view. */
    GtkTextBuffer *buf = gtk_text_view_get_buffer(g_console_view);   /* Borrowed pointer */

    /* Move an iterator to the end so we can insert at the bottom. */
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buf, &end);                         /* Position at buffer end */

    /* Insert text plus a newline (GtkTextBuffer does not auto-append). */
    gtk_text_buffer_insert(buf, &end, line, -1);                     /* Insert provided UTF-8 */
    gtk_text_buffer_insert(buf, &end, "\n", -1);                     /* Append newline */

    /* Keep view scrolled to bottom (typical console behavior). */
    GtkTextMark *mark = gtk_text_buffer_create_mark(buf, NULL, &end, FALSE); /* Mark at end */
    gtk_text_view_scroll_mark_onscreen(g_console_view, mark);        /* Scroll to mark */
    gtk_text_buffer_delete_mark(buf, mark);                          /* Clean temp mark */
}
/* Local helper: spawn "git <argv>" with optional @cwd and capture exit/status.
 * On success:
 *   - @out_stdout is set to newly-allocated UTF-8 text (may be empty string).
 *   - returns TRUE if process exit status == 0, else FALSE.
 * On failure to spawn/communicate, returns FALSE and sets @error.
 * If @out_stdout is NULL, stdout is read and discarded.
 *
 * Notes:
 *  - We use GSubprocessLauncher so we can set a working directory (cwd).
 *  - We pipe both STDOUT and STDERR; @error includes stderr text on failures.
 */