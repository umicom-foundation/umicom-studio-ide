#ifndef UMI_GUI_APP_H
#define UMI_GUI_APP_H

#include <glib.h>
#include <gtk/gtk.h>

typedef struct _UmiEditor UmiEditor;
typedef struct _UmiFileTree UmiFileTree;
typedef struct _UmiSearchPanel UmiSearchPanel;
typedef struct _UmiStatus UmiStatus;

typedef struct {
  GtkApplication *app;
  GtkWindow      *win;
  UmiEditor      *ed;
  UmiFileTree    *tree;
  UmiSearchPanel *search;
  UmiStatus      *status;
  gpointer        reserved;
} UmiApp;

GtkApplication *umi_app_new(void);
UmiApp *umi_app_handle(GtkApplication *app);
GtkWindow *umi_app_window(UmiApp *ua);
UmiEditor *umi_app_editor(UmiApp *ua);

#endif /* UMI_GUI_APP_H */
