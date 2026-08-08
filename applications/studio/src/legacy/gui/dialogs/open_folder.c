/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/dialog/open_folder.c
 *
 * PURPOSE:
 *   Synchronous “Open Folder” prompt implemented on top of GTK4’s async
 *   GtkFileDialog. We use a small GMainLoop to emulate a blocking call while
 *   keeping the API simple for the caller.
 *
 * DESIGN:
 *   - No deep includes: "workspace.h" is included via open_folder.h.
 *   - All allocations are freed; main loop is always unref’d.
 *
 * SECURITY/ROBUSTNESS:
 *   - All pointers guarded (parent/window/workspace).
 *   - No unchecked string ops; GIO handles paths safely.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include "open_folder.h"
#include <gio/gio.h>
#include <gtk/gtk.h>

/* Small context passed through the async completion to set workspace root. */
typedef struct {
  UmiWorkspace *ws;
  GMainLoop    *loop;
} OpenFolderCtx;

static void on_select_folder_cb(GObject *source, GAsyncResult *res, gpointer user_data)
{
  GtkFileDialog *dlg = GTK_FILE_DIALOG(source);
  OpenFolderCtx *ctx = (OpenFolderCtx*)user_data;

  g_autoptr(GError) err = NULL;
  g_autoptr(GFile)  file = gtk_file_dialog_select_folder_finish(dlg, res, &err);

  if (!err && file && ctx && ctx->ws) {
    g_autofree char *path = g_file_get_path(file);
    if (path && *path) umi_workspace_set_root(ctx->ws, path);
  }

  if (ctx && ctx->loop) g_main_loop_quit(ctx->loop);
}

void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws)
{
  if (!ws) return;

  GtkFileDialog *dlg = gtk_file_dialog_new();
  OpenFolderCtx ctx = { .ws = ws, .loop = g_main_loop_new(NULL, FALSE) };

  /* Kick the async selection; completion updates workspace + quits loop. */
  gtk_file_dialog_select_folder(dlg, parent, NULL, on_select_folder_cb, &ctx);

  /* Block here until the async finishes (keeps public API synchronous). */
  g_main_loop_run(ctx.loop);

  g_main_loop_unref(ctx.loop);
  g_object_unref(dlg);
}
