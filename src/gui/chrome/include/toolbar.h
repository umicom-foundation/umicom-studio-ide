/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/toolbar.h
 * PURPOSE: Toolbar with run/stop/save/quick-open entries
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TOOLBAR_H
#define UMICOM_TOOLBAR_H

#include <gtk/gtk.h>

typedef struct _UmiToolbar UmiToolbar;

typedef void (*UmiNoArgCb)(gpointer user);

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
