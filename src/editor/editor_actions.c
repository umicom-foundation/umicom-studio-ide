/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/editor/editor_actions.c
 *
 * PURPOSE:
 *   Minimal editor operations (open/save/new) implemented with GLib/GTK only
 *   and no extra UI plumbing yet. These functions are safe, bounded, and
 *   return errors via GError** as per GLib conventions.
 *
 * DESIGN:
 *   - No dependency on status bars, dialogs, or other modules.
 *   - Uses a detached GtkTextBuffer owned by UmiEditor.
 *
 * SECURITY/ROBUSTNESS:
 *   - Validates inputs and returns G_IO_ERROR codes when invalid.
 *   - Bounds copies; avoids raw sprintf; frees allocations.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "editor_actions.h"   /* public prototypes */

static GtkTextBuffer* ensure_buffer(UmiEditor *ed)
{
    if (!ed) return NULL;
    if (!ed->buffer) ed->buffer = gtk_text_buffer_new(NULL);
    return ed->buffer;
}

gboolean umi_editor_open_file(UmiEditor *ed, const char *path, GError **err)
{
    if (!ed || !path || !*path) {
        if (err) g_set_error(err, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                             "umi_editor_open_file: invalid editor or path");
        return FALSE;
    }

    gchar *txt = NULL;
    gsize len = 0;
    if (!g_file_get_contents(path, &txt, &len, err)) {
        return FALSE; /* 'err' already set */
    }

    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!buf) { g_free(txt); return FALSE; }

    gtk_text_buffer_set_text(buf, txt, (gint)len);

    g_free(ed->current_file);
    ed->current_file = g_strdup(path);

    g_message("Editor: opened '%s' (%" G_GSIZE_FORMAT " bytes)", path, len);
    g_free(txt);
    return TRUE;
}

gboolean umi_editor_save(UmiEditor *ed, GError **err)
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
    if (ok) g_message("Editor: saved '%s'", ed->current_file);
    g_free(txt);
    return ok;
}

gboolean umi_editor_save_as_path(UmiEditor *ed, const char *path, GError **err)
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

gboolean umi_editor_save_as(UmiEditor *ed, GError **err)
{
    (void)ed;
    if (err) *err = g_error_new_literal(G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                                        "Save As dialog not implemented yet");
    return FALSE;
}

void umi_editor_new_file(UmiEditor *ed)
{
    GtkTextBuffer *buf = ensure_buffer(ed);
    if (!ed || !buf) return;

    gtk_text_buffer_set_text(buf, "", -1);
    g_clear_pointer(&ed->current_file, g_free);
    g_message("Editor: new file");
}
