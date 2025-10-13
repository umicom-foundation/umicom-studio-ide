/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app.c
 *
 * PURPOSE:
 *   Implement the top-level GTK application shell with minimal dependencies.
 *   Provides a window, a very small toolbar, and placeholders for editor and
 *   file tree panes. Actions are wired in a decoupled way (weak refs).
 *
 * DESIGN:
 *   - Weak function refs let this compile/link even if editor/run modules
 *     are not yet included in a particular build configuration.
 *   - No deprecated GTK APIs (GTK4 only).
 *   - No deep includes; headers by name only.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include "app.h"

/* Forward declare editor type to avoid heavy includes. */
typedef struct _UmiEditor UmiEditor;

/* Weak cross-module hooks (portable guards) */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) gboolean umi_run_pipeline_start(gpointer out, gpointer problems, gpointer reserved);
__attribute__((weak)) void     umi_run_pipeline_stop(void);
__attribute__((weak)) gboolean umi_editor_save(UmiEditor *ed, GError **error);
#else
/* On non-GNU/Clang toolchains, omit weak refs and fall back to NULL checks. */
gboolean (*umi_run_pipeline_start)(gpointer,gpointer,gpointer) = NULL;
void     (*umi_run_pipeline_stop)(void) = NULL;
gboolean (*umi_editor_save)(UmiEditor*,GError**) = NULL;
#endif

/* Keep one UmiApp per GtkApplication */
static GHashTable *g_map = NULL;

/* Actions (small wrappers) */
static void do_run(gpointer u){
  (void)u;
  if (umi_run_pipeline_start) (void)umi_run_pipeline_start(NULL, NULL, NULL);
}
static void do_stop(gpointer u){
  (void)u;
  if (umi_run_pipeline_stop) umi_run_pipeline_stop();
}
static void do_save(gpointer u){
  UmiApp *ua = (UmiApp*)u;
  if (umi_editor_save && ua && ua->ed) {
    GError *e = NULL;
    if (!umi_editor_save(ua->ed, &e) && e) g_error_free(e);
  }
}

/* Tiny toolbar */
static GtkWidget* make_toolbar(UmiApp *ua){
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *btn_run  = gtk_button_new_with_label("Run");
  GtkWidget *btn_stop = gtk_button_new_with_label("Stop");
  GtkWidget *btn_save = gtk_button_new_with_label("Save");
  gtk_box_append(GTK_BOX(box), btn_run);
  gtk_box_append(GTK_BOX(box), btn_stop);
  gtk_box_append(GTK_BOX(box), btn_save);
  g_signal_connect_swapped(btn_run,  "clicked", G_CALLBACK(do_run),  ua);
  g_signal_connect_swapped(btn_stop, "clicked", G_CALLBACK(do_stop), ua);
  g_signal_connect_swapped(btn_save, "clicked", G_CALLBACK(do_save), ua);
  return box;
}

/* Lifecycle */
static void on_activate(GtkApplication *app, gpointer user)
{
  (void)user;
  if (!g_map) g_map = g_hash_table_new(g_direct_hash, g_direct_equal);

  UmiApp *ua = g_hash_table_lookup(g_map, app);
  if (!ua) {
    ua = g_new0(UmiApp, 1);
    ua->app = app;
    g_hash_table_insert(g_map, app, ua);
  }

  ua->win = GTK_WINDOW(gtk_application_window_new(app));
  gtk_window_set_title(ua->win, "Umicom Studio IDE");
  gtk_window_set_default_size(ua->win, 1200, 800);

  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  ua->root = root;

  gtk_box_append(GTK_BOX(root), make_toolbar(ua));

  GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_append(GTK_BOX(root), hpaned);

  GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_box_append(GTK_BOX(left), gtk_label_new("File Tree"));
  gtk_paned_set_start_child(GTK_PANED(hpaned), left);

  GtkWidget *editor_placeholder = gtk_label_new("Editor");
  gtk_paned_set_end_child(GTK_PANED(hpaned), editor_placeholder);

  GtkWidget *status = gtk_label_new("Status");
  gtk_box_append(GTK_BOX(root), status);

  gtk_window_set_child(ua->win, root);
  gtk_window_present(ua->win);
}

static void on_startup(GtkApplication *app, gpointer user)
{
  (void)app; (void)user;
  if (!g_map) g_map = g_hash_table_new(g_direct_hash, g_direct_equal);
}

/* Public API */
GtkApplication *umi_app_new(void)
{
  GtkApplication *app = gtk_application_new("org.umicom.studio", G_APPLICATION_HANDLES_OPEN);
  g_signal_connect(app, "startup",  G_CALLBACK(on_startup),  NULL);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return app;
}

UmiApp *umi_app_handle(GtkApplication *app)
{
  if (!g_map) return NULL;
  return g_hash_table_lookup(g_map, app);
}

GtkWindow *umi_app_window(UmiApp *ua)  { return ua ? ua->win : NULL; }
UmiEditor *umi_app_editor(UmiApp *ua)  { return ua ? ua->ed  : NULL; }
