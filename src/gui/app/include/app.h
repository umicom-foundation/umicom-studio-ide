/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app.h
 * PURPOSE: Public app shell types and accessors for the top-level GTK app.
* Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *
 * Plain-English notes:
 * - This header exposes tiny, easy-to-understand functions that do 2 things:
 *   (1) apply the app's CSS theme to a GTK window/display, and
 *   (2) read the embedded JSON preset data as a string.
 * - We keep this API small so it's easy to use and hard to misuse.
 * - Resource files are compiled into the app via ustudio.gresource.xml.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APP_H
#define UMICOM_APP_H

#include <gtk/gtk.h>
#include <glib.h>

G_BEGIN_DECLS

/* Forward decl to avoid hard dependency; actual type lives in editor headers. */
typedef struct _UmiEditor UmiEditor;

/* Public app shell structure kept intentionally tiny. */
typedef struct UmiApp {
  GtkApplication *app;   /* owning GtkApplication                         */
  GtkWindow      *win;   /* main window                                   */
  GtkWidget      *root;  /* root container widget                         */
  UmiEditor      *ed;    /* current editor instance (may be NULL)         */
} UmiApp;

/* Create a new GtkApplication with our handlers wired. */
GtkApplication *umi_app_new(void);

/* Lookup the per-application UmiApp handle (or NULL if not created yet). */
UmiApp    *umi_app_handle(GtkApplication *app);

/* Convenience accessors. */
GtkWindow *umi_app_window(UmiApp *ua);
UmiEditor *umi_app_editor(UmiApp *ua);

G_END_DECLS

#endif /* UMICOM_APP_H */
/*--- end of file ---*/