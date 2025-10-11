/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/prefs_ui.c
 * PURPOSE: Implementation of a simple preferences dialog
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/prefs_ui.h"

static void load_values(UmiPrefsUI *ui){
  const char *theme = umi_json_get(ui->store, "theme");
  const char *font  = umi_json_get(ui->store, "font_size");
  gtk_editable_set_text(GTK_EDITABLE(ui->theme_entry), theme?theme:"Dark");
  int sz = font ? atoi(font) : 14;
  gtk_spin_button_set_value(ui->font_spin, sz);
}

static void on_save(GtkButton *b, gpointer user){
  (void)b;
  UmiPrefsUI *ui = (UmiPrefsUI*)user;
  const char *theme = gtk_editable_get_text(GTK_EDITABLE(ui->theme_entry));
  int sz = (int)gtk_spin_button_get_value(ui->font_spin);

  /* Persist to JSON */
  umi_json_set(ui->store, "theme", theme?theme:"Dark");
  char buf[32]; g_snprintf(buf, sizeof(buf), "%d", sz);
  umi_json_set(ui->store, "font_size", buf);
  GError *e=NULL; if(!umi_json_save(ui->store, &e)){ g_warning("prefs save: %s", e?e->message:"(unknown)"); if(e) g_error_free(e); }

  /* Emit live changes */
  UmiSettingsBus *bus = umi_settings_bus_get();
  umi_settings_emit(bus, "theme", theme?theme:"Dark");
  umi_settings_emit(bus, "font_size", buf);

  gtk_window_close(GTK_WINDOW(ui->dialog));
}

UmiPrefsUI *umi_prefs_create(GtkWindow *parent, const char *json_path){
  UmiPrefsUI *ui = g_new0(UmiPrefsUI,1);
  ui->store = umi_json_store_load(json_path?json_path:"config/settings.json");

  ui->dialog = gtk_dialog_new_with_buttons("Preferences", parent, 0, "Save", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);

  GtkWidget *row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row1), gtk_label_new("Theme:"));
  ui->theme_entry = GTK_ENTRY(gtk_entry_new());
  gtk_box_append(GTK_BOX(row1), GTK_WIDGET(ui->theme_entry));

  GtkWidget *row2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row2), gtk_label_new("Font Size:"));
  ui->font_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(8, 48, 1));
  gtk_box_append(GTK_BOX(row2), GTK_WIDGET(ui->font_spin));

  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(ui->dialog));
  gtk_box_append(GTK_BOX(box), row1);
  gtk_box_append(GTK_BOX(box), row2);
  gtk_box_append(GTK_BOX(content), box);

  load_values(ui);

  /* Wire the Save button explicitly so beginners can see the flow. */
  GtkWidget *save_btn = gtk_dialog_get_widget_for_response(GTK_DIALOG(ui->dialog), GTK_RESPONSE_OK);
  g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save), ui);

  return ui;
}

void umi_prefs_show(UmiPrefsUI *ui){
  if(!ui) return;
  gtk_window_present(GTK_WINDOW(ui->dialog));
}
