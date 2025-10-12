/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor_actions.c
 * PURPOSE: Minimal editor operations (open/save/new) with zero dependency on
 *          status APIs. Pure C (C17) + GLib/GTK; no UI/status/XML required.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>

#include "editor_actions.h"   /* public prototypes & UmiEditor from editor.h */

/* Internal helper: ensure the editor has a text buffer.
 * Creates a default GtkTextBuffer if missing, and returns it (or NULL on fail). */
static GtkTextBuffer* ensure_buffer(UmiEditor *ed)
{
    if (!ed) return NULL;
    if (!ed->buffer) {
        ed->buffer = gtk_text_buffer_new(NULL);
    }
    return ed->buffer;
}

/* Open a file from disk and load its contents into the editor buffer.
 * On success, updates ed->current_file and logs the action.
 * Errors are returned via GError** as per GLib conventions. */
gboolean
umi_editor_open_file(UmiEditor *ed, const char *path, GError **err)
{
    if (!ed || !path || !*path) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                             "umi_editor_open_file: invalid editor or path");
        return FALSE;
    }

    gchar *txt = NULL;
    gsize len = 0;
    if (!g_file_get_contents(path, &txt, &len, err)) {
        return FALSE; /* 'err' is already set by g_file_get_contents */
    }

    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!buf) { g_free(txt); return FALSE; }

    gtk_text_buffer_set_text(buf, txt, (gint)len);

    g_free(ed->current_file);
    ed->current_file = g_strdup(path);

    /* No status dependency: log to console instead. */
    g_message("Editor: opened '%s' (%" G_GSIZE_FORMAT " bytes)", path, len);

    g_free(txt);
    return TRUE;
}

/* Save the current buffer to the current file path.
 * Returns FALSE if there is no current_file or buffer. */
gboolean
umi_editor_save(UmiEditor *ed, GError **err)
{
    if (!ed || !ed->current_file) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                             "umi_editor_save: no current file");
        return FALSE;
    }

    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!buf) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_FAILED,
                             "umi_editor_save: no buffer");
        return FALSE;
    }

    GtkTextIter s, e;
    gtk_text_buffer_get_bounds(buf, &s, &e);
    gchar *txt = gtk_text_buffer_get_text(buf, &s, &e, FALSE);

    gboolean ok = g_file_set_contents(ed->current_file, txt, -1, err);
    if (ok) {
        g_message("Editor: saved '%s'", ed->current_file);
    }
    g_free(txt);
    return ok;
}

/* Save the current buffer to an explicit path (used by Save As wrappers). */
gboolean
umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err)
{
    if (!ed || !path || !*path) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                             "umi_editor_save_as_path: invalid editor or path");
        return FALSE;
    }

    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!buf) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_FAILED,
                             "umi_editor_save_as_path: no buffer");
        return FALSE;
    }

    GtkTextIter s, e;
    gtk_text_buffer_get_bounds(buf, &s, &e);
    gchar *txt = gtk_text_buffer_get_text(buf, &s, &e, FALSE);

    gboolean ok = g_file_set_contents(path, txt, -1, err);
    if (ok) {
        g_free(ed->current_file);
        ed->current_file = g_strdup(path);
        g_message("Editor: saved-as '%s'", path);
    }
    g_free(txt);
    return ok;
}

/* Save-As placeholder (no dialog yet); returns “not supported”. */
gboolean
umi_editor_save_as(UmiEditor *ed, GError **err)
{
    (void)ed;
    if (err) {
        *err = g_error_new_literal(G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                                   "Save As dialog not implemented yet");
    }
    return FALSE;
}

/* Create a new, empty, untitled document. */
void
umi_editor_new_file(UmiEditor *ed)
{
    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!ed || !buf) return;

    gtk_text_buffer_set_text(buf, "", -1);
    g_clear_pointer(&ed->current_file, g_free);

    /* No status dependency: just log. */
    g_message("Editor: new file");
}
/*--- end of file ---*/