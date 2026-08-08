/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs/prefs_ui.c
 *
 * PURPOSE:
 *   Modern GTK4 Preferences window (GtkWindow with HeaderBar) that loads/
 *   saves two basic settings (theme + font size) using a simple JSON store
 *   and broadcasts live changes via the settings bus.
 *
 * DESIGN:
 *   - No deprecated GtkDialog APIs.
 *   - All headers included by name (no deep relative includes).
 *   - Defensive checks and explicit ownership.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>         /* GTK core                                      */
#include <glib.h>            /* GLib utility                                  */
#include "prefs_ui.h"        /* controller API (window, widgets, store)       */
#include "json_store.h"      /* umi_json_* helpers                            */
#include "settings_bus.h"    /* umi_settings_* event bus                      */

/* Load values from JSON store into the controls (defaults if absent).        */
static void
load_values(UmiPrefsUI *ui)
{
  if (!ui || !ui->store || !ui->theme_entry || !ui->font_spin) return;

  const char *theme = umi_json_get(ui->store, "theme");
  const char *font  = umi_json_get(ui->store, "font_size");

  gtk_editable_set_text(GTK_EDITABLE(ui->theme_entry),
                        theme ? theme : "Dark");

  int sz = (font && *font) ? (int)g_ascii_strtoll(font, NULL, 10) : 14;
  gtk_spin_button_set_value(ui->font_spin, sz);
}

/* Save handler: persist JSON, emit live change events, and close the window. */
static void
on_save(GtkButton *btn, gpointer user_data)
{
  (void)btn;
  UmiPrefsUI *ui = (UmiPrefsUI*)user_data;
  if (!ui || !ui->store) return;

  const char *theme = gtk_editable_get_text(GTK_EDITABLE(ui->theme_entry));

  /* Save theme string */
  umi_json_set(ui->store, "theme", theme ? theme : "Dark");

  /* Save font size as string */
  char buf[32];
  g_snprintf(buf, sizeof(buf), "%d", (int)gtk_spin_button_get_value(ui->font_spin));
  umi_json_set(ui->store, "font_size", buf);

  /* Persist to disk */
  GError *e = NULL;
  if (!umi_json_save(ui->store, &e)) {
    g_warning("prefs: save failed: %s", e ? e->message : "(unknown)");
    if (e) g_error_free(e);
  }

  /* Broadcast live changes to the app */
  UmiSettingsBus *bus = umi_settings_bus_get();
  umi_settings_emit(bus, "theme", theme ? theme : "Dark");
  umi_settings_emit(bus, "font_size", buf);

  gtk_window_close(ui->window);
}

/* Create a modern GTK4 prefs window with headerbar and a simple form.        */
UmiPrefsUI *
umi_prefs_create(GtkWindow *parent, const char *json_path)
{
  UmiPrefsUI *ui = g_new0(UmiPrefsUI, 1);

  /* Backing store (file path or default when NULL) */
  ui->store = umi_json_store_load(json_path ? json_path : "config/settings.json");

  /* Window + headerbar */
  ui->window = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(ui->window, "Preferences");
  if (parent) gtk_window_set_transient_for(ui->window, parent);
  gtk_window_set_modal(ui->window, TRUE);

  GtkWidget *hb = gtk_header_bar_new();
  gtk_window_set_titlebar(ui->window, hb);

  GtkWidget *btn_cancel = gtk_button_new_with_label("Cancel");
  GtkWidget *btn_ok     = gtk_button_new_with_label("Save");
  gtk_header_bar_pack_start(GTK_HEADER_BAR(hb), btn_cancel);
  gtk_header_bar_pack_end  (GTK_HEADER_BAR(hb), btn_ok);

  /* Content: simple two-row form */
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

  GtkWidget *row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row1), gtk_label_new("Theme:"));
  ui->theme_entry = GTK_ENTRY(gtk_entry_new());
  gtk_box_append(GTK_BOX(row1), GTK_WIDGET(ui->theme_entry));

  GtkWidget *row2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row2), gtk_label_new("Font Size:"));
  ui->font_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(8, 48, 1));
  gtk_box_append(GTK_BOX(row2), GTK_WIDGET(ui->font_spin));

  gtk_box_append(GTK_BOX(root), row1);
  gtk_box_append(GTK_BOX(root), row2);

  gtk_window_set_child(ui->window, root);

  /* Initial values */
  load_values(ui);

  /* Signals */
  g_signal_connect_swapped(btn_cancel, "clicked",
                           G_CALLBACK(gtk_window_close), ui->window);
  g_signal_connect(btn_ok, "clicked", G_CALLBACK(on_save), ui);

  return ui;
}

/* Present the window on screen.                                              */
void
umi_prefs_show(UmiPrefsUI *ui)
{
  if (!ui || !ui->window) return;
  gtk_window_present(ui->window);
}

/* Destroy controller and window (safe on NULL).                              */
void
umi_prefs_destroy(UmiPrefsUI *ui)
{
  if (!ui) return;
  if (ui->window) gtk_window_destroy(ui->window);
  /* ui->store is owned by json_store; if ownership is ours, free accordingly */
  g_free(ui);
}
