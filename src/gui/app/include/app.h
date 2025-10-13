/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app.h
 *
 * PURPOSE:
 *   Public app shell types and accessors for the top-level GTK application.
 *
 * API:
 *   GtkApplication *umi_app_new(void);
 *   UmiApp        *umi_app_handle(GtkApplication *app);
 *   GtkWindow     *umi_app_window(UmiApp *ua);
 *   struct _UmiEditor *umi_app_editor(UmiApp *ua);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APP_H
#define UMICOM_APP_H

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

/* forward decl to avoid hard dependency here */
typedef struct _UmiEditor UmiEditor;

typedef struct UmiApp {
  GtkApplication *app;   /* owning application */
  GtkWindow      *win;   /* main toplevel     */
  GtkWidget      *root;  /* root container    */
  UmiEditor      *ed;    /* editor instance   */
} UmiApp;

GtkApplication *umi_app_new(void);
UmiApp         *umi_app_handle(GtkApplication *app);
GtkWindow      *umi_app_window(UmiApp *ua);
UmiEditor      *umi_app_editor(UmiApp *ua);

G_END_DECLS
#endif /* UMICOM_APP_H */
