/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/app_actions.h
 * PURPOSE: Application-level action callbacks and helpers used by menus & UI
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_APP_ACTIONS_H
#define UMI_APP_ACTIONS_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations to avoid heavy includes in headers */
typedef struct _UmiApp         UmiApp;
typedef struct _UmiEditor      UmiEditor;
typedef struct _UmiStatus      UmiStatus;
typedef struct _UmiStatusBar   UmiStatusBar;
typedef struct _UmiSearchPanel UmiSearchPanel;

/* Action entry points (wire these to menus, accelerators, etc.) */
void umi_action_palette(GtkWidget *widget, gpointer user);
void umi_action_run(GtkWidget *widget, gpointer user);
void umi_action_stop(GtkWidget *widget, gpointer user);
void umi_action_focus_search(GtkWidget *widget, gpointer user);
void umi_action_save(GtkWidget *widget, gpointer user);
void umi_action_save_as(GtkWidget *widget, gpointer user);

#ifdef __cplusplus
}
#endif

#endif /* UMI_APP_ACTIONS_H */
