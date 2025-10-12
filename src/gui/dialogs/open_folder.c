/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/open_folder.c
 * PURPOSE: Implementation of open-folder prompt helper (GTK4)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#include "open_folder.h"
#include <gio/gio.h>          /* GFile */
#include <gtk/gtk.h>          /* GtkFileDialog (GTK4) */

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
  if (!err && file) {
    g_autofree char *path = g_file_get_path(file);
    if (path && ctx->ws) {
      umi_workspace_set_root(ctx->ws, path);
    }
  }
  if (ctx->loop) g_main_loop_quit(ctx->loop);
}

void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws)
{
  GtkFileDialog *dlg = gtk_file_dialog_new();
  OpenFolderCtx ctx = { .ws = ws, .loop = g_main_loop_new(NULL, FALSE) };

  gtk_file_dialog_select_folder(dlg, parent, NULL, on_select_folder_cb, &ctx);
  /* block until the async completes to keep a synchronous API */
  g_main_loop_run(ctx.loop);

  g_main_loop_unref(ctx.loop);
  g_object_unref(dlg);
}
