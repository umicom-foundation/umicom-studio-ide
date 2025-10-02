/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/open_folder.c
 * PURPOSE: Implementation of open-folder prompt helper
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "open_folder.h"

void umi_open_folder_prompt(GtkWindow *parent, UmiWorkspace *ws){
  GtkWidget *dlg = gtk_file_chooser_dialog_new("Open Folder", parent,
      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
      "_Cancel", GTK_RESPONSE_CANCEL,
      "_Open", GTK_RESPONSE_ACCEPT,
      NULL);
  if(gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT){
    char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
    if(dir){ umi_workspace_set_root(ws, dir); g_free(dir); }
  }
  gtk_window_destroy(GTK_WINDOW(dlg));
}
