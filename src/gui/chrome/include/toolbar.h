/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/toolbar.h
 *
 * PURPOSE:
 *   Legacy/simple toolbar with run/stop/save/palette buttons. Prefer
 *   src/gui/chrome/app_menu_llm.* if you’ve standardized on that bar.
 *
 * DESIGN:
 *   - Small callback vtable; caller owns user pointer.
 *   - GtkWidget* returned for packing; no GTK Builder XML required.
 *
 * API:
 *   typedef struct _UmiToolbar UmiToolbar;
 *   typedef void (*UmiNoArgCb)(gpointer user);
 *   typedef struct { ... } UmiToolbarCallbacks;
 *   UmiToolbar *umi_toolbar_new(const UmiToolbarCallbacks *cb);
 *   GtkWidget  *umi_toolbar_widget(UmiToolbar *tb);
 *
 * DELETE NOTE:
 *   If your UI now uses app_menu_llm.* exclusively, you can safely remove
 *   this header and its implementation to avoid duplicate toolbars.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLBAR_H
#define UMICOM_TOOLBAR_H

#include <gtk/gtk.h>

typedef struct _UmiToolbar UmiToolbar;
typedef void (*UmiNoArgCb)(gpointer user);

/* Callbacks are optional; pass NULL members to omit a button’s action. */
typedef struct {
  UmiNoArgCb on_run;
  UmiNoArgCb on_stop;
  UmiNoArgCb on_save;
  UmiNoArgCb on_palette;
  gpointer   user;
} UmiToolbarCallbacks;

UmiToolbar *umi_toolbar_new(const UmiToolbarCallbacks *cb);
GtkWidget  *umi_toolbar_widget(UmiToolbar *tb);

#endif /* UMICOM_TOOLBAR_H */
