/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/palette.c
 *
 * PURPOSE:
 *   Theme Palette implementation using modern GTK4 widgets. Presents a small
 *   window (not deprecated GtkDialog) with content you can expand later
 *   (e.g., accent color preview, theme preset selector, etc.).
 *
 * DESIGN:
 *   - Opaque UmiThemePalette holds a GtkWindow* and its content.
 *   - No deprecated APIs; no deep relative include paths.
 *   - All pointers and strings are guarded defensively.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>          /* GTK4 core: GtkWindow, HeaderBar, etc. */
#include <glib.h>
#include "palette.h"          /* public declarations only               */

struct UmiThemePalette {
  GtkWindow *win;             /* dedicated window for theme palette    */
  GtkWidget *root;            /* main container                        */
  GtkWidget *accent_entry;    /* simple accent color entry (text)      */
  GtkWidget *apply_btn;       /* apply button                          */
};

/* Forward decl for live theme helper (optional dependency via weak ref). */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak))
#endif
void umi_theme_live_set_accent(const char *rgba);

/* Build UI safely with modern GTK4 primitives. */
static void build_ui(UmiThemePalette *p)
{
  p->win = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(p->win, "Theme Palette");
  gtk_window_set_modal(p->win, TRUE);
  gtk_window_set_default_size(p->win, 420, 160);

  GtkWidget *hb = gtk_header_bar_new();
  gtk_window_set_titlebar(p->win, hb);

  GtkWidget *btn_close = gtk_button_new_with_label("Close");
  g_signal_connect_swapped(btn_close, "clicked", G_CALLBACK(gtk_window_close), p->win);
  gtk_header_bar_pack_end(GTK_HEADER_BAR(hb), btn_close);

  p->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_margin_start (p->root, 12);
  gtk_widget_set_margin_end   (p->root, 12);
  gtk_widget_set_margin_top   (p->root, 12);
  gtk_widget_set_margin_bottom(p->root, 12);

  /* Simple row to set an accent color live (delegates to theme_live). */
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(row), gtk_label_new("Accent (CSS color):"));
  p->accent_entry = gtk_entry_new();
  gtk_editable_set_text(GTK_EDITABLE(p->accent_entry), "#4a90e2");
  gtk_box_append(GTK_BOX(row), p->accent_entry);

  p->apply_btn = gtk_button_new_with_label("Apply");
  gtk_box_append(GTK_BOX(row), p->apply_btn);

  gtk_box_append(GTK_BOX(p->root), row);
  gtk_window_set_child(p->win, p->root);

  /* Wire button: call live theming helper if linked. */
  g_signal_connect_swapped(p->apply_btn, "clicked", G_CALLBACK(+[] (UmiThemePalette *self) {
    if (!self) return;
    const char *txt = gtk_editable_get_text(GTK_EDITABLE(self->accent_entry));
    if (umi_theme_live_set_accent) umi_theme_live_set_accent(txt);
  }), p);
}

/* Public API ----------------------------------------------------------------*/

UmiThemePalette *umi_theme_palette_new(void)
{
  UmiThemePalette *p = g_new0(UmiThemePalette, 1);
  build_ui(p);
  return p;
}

void umi_theme_palette_open(UmiThemePalette *p, GtkWindow *parent)
{
  if (!p || !p->win) return;
  if (parent) gtk_window_set_transient_for(p->win, parent);
  gtk_window_present(p->win);
}

void umi_theme_palette_free(UmiThemePalette *p)
{
  if (!p) return;
  if (p->win) gtk_window_destroy(p->win);
  g_free(p);
}
