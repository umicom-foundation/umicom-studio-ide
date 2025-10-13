/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/prefs/prefs_ui.c
 * PURPOSE: Implementation of a simple preferences dialog
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "prefs_ui.h"  /* Declares UmiPrefsUI and JSON helpers used below */
#include <stdlib.h>            /* atoi                                            */
#include <string.h>            /* for safety if future string ops are added       */

/*-----------------------------------------------------------------------------
 * load_values:
 * Populate the UI controls from the backing store (JSON key/value file).
 * - If a key is missing, we set a reasonable default so the UI always starts
 *   with something valid (e.g., "Dark" theme, font size 14).
 *---------------------------------------------------------------------------*/
static void load_values(UmiPrefsUI *ui){
  /* Defensive: tolerate NULL in case caller passed an uninitialized struct.  */
  if(!ui || !ui->store || !ui->theme_entry || !ui->font_spin) return;

  /* Read strings from the JSON store; NULL when key is not present.         */
  const char *theme = umi_json_get(ui->store, "theme");
  const char *font  = umi_json_get(ui->store, "font_size");

  /* Apply theme to entry (fallback "Dark" keeps UI consistent).              */
  gtk_editable_set_text(GTK_EDITABLE(ui->theme_entry), theme ? theme : "Dark");

  /* Convert stored font_size (string) to int; default to 14 if absent.       */
  int sz = font ? atoi(font) : 14;

  /* Set the spin button value (GTK uses double internally; we pass int).     */
  gtk_spin_button_set_value(ui->font_spin, sz);
}

/*-----------------------------------------------------------------------------
 * on_save (signal handler for the dialog's Save/OK button):
 * Collects values from the UI, persists changes to the JSON store, emits
 * "live" settings change notifications (so the rest of the app can react),
 * and closes the dialog.
 * Notes:
 *  - Parameter @b (the button) is unused in this implementation, so we cast
 *    it to void to avoid compiler warnings.
 *  - Errors from umi_json_save are logged via g_warning() but do not crash.
 *---------------------------------------------------------------------------*/
static void on_save(GtkButton *b, gpointer user){
  (void)b; /* suppress -Wunused-parameter warning, keeping signature GTK-friendly */

  /* Recover our controller object from user data and validate.               */
  UmiPrefsUI *ui = (UmiPrefsUI*)user;
  if(!ui) return;

  /* Pull current values from the widgets.                                    */
  const char *theme = gtk_editable_get_text(GTK_EDITABLE(ui->theme_entry));
  int sz = (int)gtk_spin_button_get_value(ui->font_spin);

  /* Persist to JSON: we store "theme" and "font_size" as strings.            */
  umi_json_set(ui->store, "theme", theme ? theme : "Dark");

  char buf[32];
  g_snprintf(buf, sizeof(buf), "%d", sz);
  umi_json_set(ui->store, "font_size", buf);

  /* Attempt to save; report (but don’t abort) on failure.                    */
  GError *e = NULL;
  if(!umi_json_save(ui->store, &e)){
    g_warning("prefs save failed: %s", e ? e->message : "(unknown)");
    if(e) g_error_free(e);
  }

  /* Broadcast live-changes so other subsystems (theme, editor) can react.    */
  UmiSettingsBus *bus = umi_settings_bus_get();           /* singleton/event bus */
  umi_settings_emit(bus, "theme", theme ? theme : "Dark");/* e.g., restyle UI    */
  umi_settings_emit(bus, "font_size", buf);               /* e.g., adjust fonts  */

  /* Close the dialog now that we’re done.                                    */
  gtk_window_close(GTK_WINDOW(ui->dialog));
}

/*-----------------------------------------------------------------------------
 * umi_prefs_create:
 * Create the preferences dialog UI and initialize it from a JSON path.
 * Parameters:
 *   - parent: optional transient parent (for stacking/focus behavior)
 *   - json_path: optional override for settings file; defaults to
 *                "config/settings.json" when NULL.
 * Returns:
 *   - Newly allocated UmiPrefsUI*, which the caller owns and must destroy
 *     using the corresponding free/destroy function declared in prefs_ui.h.
 *---------------------------------------------------------------------------*/
UmiPrefsUI *umi_prefs_create(GtkWindow *parent, const char *json_path){
  /* Allocate controller struct with all fields zeroed for safety.            */
  UmiPrefsUI *ui = g_new0(UmiPrefsUI, 1);

  /* Load (or create) the JSON store backing these controls.                  */
  ui->store = umi_json_store_load(json_path ? json_path : "config/settings.json");

  /* Create a simple dialog with “Save” and “Cancel” buttons.                 */
  ui->dialog = G_GNUC_BEGIN_IGNORE_DEPRECATIONS
gtk_dialog_new_with_buttons("Preferences", parent,
                                           0 /* flags */,
                                           "Save",   GTK_RESPONSE_OK,
                                           "Cancel", GTK_RESPONSE_CANCEL,
                                           NULL);

  /* Work area: a vertical box that hosts two rows (theme + font size).       */
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8 /* spacing */);
  gtk_box_set_homogeneous(GTK_BOX(box), FALSE);

  /* Row 1: Theme entry (free text for now; could be a combo later).          */
  GtkWidget *row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row1), gtk_label_new("Theme:"));
  ui->theme_entry = GTK_ENTRY(gtk_entry_new());
  gtk_box_append(GTK_BOX(row1), GTK_WIDGET(ui->theme_entry));

  /* Row 2: Font size spin (range 8..48, step 1).                             */
  GtkWidget *row2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row2), gtk_label_new("Font Size:"));
  ui->font_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(8, 48, 1));
  gtk_box_append(GTK_BOX(row2), GTK_WIDGET(ui->font_spin));

  /* Pack rows into the dialog’s content area.                                */
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(ui->dialog));
  gtk_box_append(GTK_BOX(box), row1);
  gtk_box_append(GTK_BOX(box), row2);
  gtk_box_append(GTK_BOX(content), box);

  /* Initialize the controls with current (or default) values.                */
  load_values(ui);

  /* Wire the explicit Save button click so flow is easy to follow/maintain.  */
  GtkWidget *save_btn =
    G_GNUC_BEGIN_IGNORE_DEPRECATIONS
    gtk_dialog_get_widget_for_response(GTK_DIALOG(ui->dialog), GTK_RESPONSE_OK);
G_GNUC_END_IGNORE_DEPRECATIONS
G_GNUC_END_IGNORE_DEPRECATIONS
  g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save), ui);

  return ui;
}
