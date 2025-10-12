/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/palette.c
 * PURPOSE: Implementation of a tiny command palette (GTK4)
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab
 * Date: 2025-10-01
 * License: MIT
 *
 * Why this file exists:
 *  - Provide a minimal, dependency-light command palette with search.
 *  - Keep it SELF-CONTAINED: no project-wide "utf8.h" or other helpers.
 *  - Only depends on GTK/GLib, so the module can be moved/trimmed easily.
 *
 * Key behaviors:
 *  - A GtkDialog with an entry and a list of buttons (each button = command).
 *  - Typing in the entry filters commands by case-insensitive UTF-8 match.
 *  - Clicking a button invokes the command callback with its user pointer.
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <string.h>        /* for NULL checks only                            */
#include "include/palette.h"

/*-----------------------------------------------------------------------------
 * Local helper: utf8_icontains
 * Case-insensitive "needle in haystack" for UTF-8 strings using GLib.
 * Implementation strategy:
 *   1) Case-fold both strings via g_utf8_casefold (Unicode-aware).
 *   2) Use g_strstr_len to search the folded haystack for the folded needle.
 * Returns TRUE if @needle is empty (common UX choice).
 *---------------------------------------------------------------------------*/
static gboolean
utf8_icontains(const char *haystack, const char *needle)
{
  if (!needle || !*needle) return TRUE;   /* empty filter matches everything */
  if (!haystack) return FALSE;

  gchar *h = g_utf8_casefold(haystack, -1);
  gchar *n = g_utf8_casefold(needle,   -1);

  gboolean found = (h && n && g_strstr_len(h, -1, n) != NULL);

  g_free(h);
  g_free(n);
  return found;
}

/*-----------------------------------------------------------------------------
 * rebuild_list:
 * Completely repopulate the GtkListBox from the current command table and
 * optional filter text. Each row is a GtkButton labeled with the command name.
 * Clicking a row triggers the command's callback with its user pointer.
 *---------------------------------------------------------------------------*/
static void
rebuild_list(UmiPalette *p, const char *filter)
{
  if (!p || !p->list) return;

  /* Fast clear (GTK 4): removes all children from the list box. */
  gtk_list_box_remove_all(p->list);

  if (!p->cmds) return;

  for (guint i = 0; i < p->cmds->len; ++i) {
    UmiCmd *c = (UmiCmd*)p->cmds->pdata[i];
    if (!c || !c->name) continue;

    if (!filter || utf8_icontains(c->name, filter)) {
      GtkWidget *row = gtk_button_new_with_label(c->name);

      /* When clicked, call the command function with its user pointer.     */
      if (c->fn) {
        /* Use swapped connect so the callback receives c->user as argument */
        g_signal_connect_swapped(row, "clicked", G_CALLBACK(c->fn), c->user);
      }

      gtk_list_box_append(p->list, row);
    }
  }
}

/*-----------------------------------------------------------------------------
 * on_changed:
 * Entry "changed" handler – rebuild the list with the current filter text.
 *---------------------------------------------------------------------------*/
static void
on_changed(GtkEditable *editable, gpointer user_data)
{
  UmiPalette *p = (UmiPalette*)user_data;
  if (!p || !p->entry) return;

  const char *txt = gtk_editable_get_text(editable);
  rebuild_list(p, txt);
}

/*-----------------------------------------------------------------------------
 * umi_palette_new:
 * Create the dialog, widgets, and wire up the filtering behavior.
 * Everything is kept local and explicit.
 *---------------------------------------------------------------------------*/
UmiPalette *
umi_palette_new(GtkWindow *parent)
{
  UmiPalette *p = g_new0(UmiPalette, 1);

  p->cmds = g_ptr_array_new(); /* empty table by default (caller may replace) */

  /* Small dialog window (GTK4). Note: gtk_dialog_new() is deprecated but
     acceptable in GTK4 for simple cases; we can migrate later if desired.   */
  p->dialog = gtk_dialog_new();
  if (parent) gtk_window_set_transient_for(GTK_WINDOW(p->dialog), parent);
  gtk_window_set_title(GTK_WINDOW(p->dialog), "Command Palette");

  /* Content area: vertical box with an entry + list.                         */
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(p->dialog));
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

  p->entry = GTK_ENTRY(gtk_entry_new());
  p->list  = GTK_LIST_BOX(gtk_list_box_new());

  gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(p->entry));
  gtk_box_append(GTK_BOX(vbox), GTK_WIDGET(p->list));
  gtk_box_append(GTK_BOX(content), vbox);

  /* Filter as the user types. */
  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);

  return p;
}

/*-----------------------------------------------------------------------------
 * umi_palette_set_commands:
 * Point the palette at a (caller-owned) array of UmiCmd*. We don't take
 * ownership so the caller can share or change the table freely.
 *---------------------------------------------------------------------------*/
void
umi_palette_set_commands(UmiPalette *p, GPtrArray *cmds)
{
  if (!p) return;
  p->cmds = cmds ? cmds : p->cmds;
  rebuild_list(p, NULL);
}

/*-----------------------------------------------------------------------------
 * umi_palette_open:
 * Present the dialog window. The caller should keep @p alive while shown.
 *---------------------------------------------------------------------------*/
void
umi_palette_open(UmiPalette *p)
{
  if (!p || !p->dialog) return;
  gtk_window_present(GTK_WINDOW(p->dialog));
}

/*-----------------------------------------------------------------------------
 * umi_palette_free:
 * Destroy the dialog and free the controller struct. We do NOT free the
 * commands array items because ownership remains with the caller.
 *---------------------------------------------------------------------------*/
void
umi_palette_free(UmiPalette *p)
{
  if (!p) return;

  if (p->dialog) {
    gtk_window_destroy(GTK_WINDOW(p->dialog));
    p->dialog = NULL;
  }

  if (p->cmds) {
    /* We do not free elements (UmiCmd*) because they are caller-owned. */
    g_ptr_array_free(p->cmds, TRUE /* free container only */);
    p->cmds = NULL;
  }

  p->entry = NULL;
  p->list  = NULL;

  g_free(p);
}
/*--- end of file ---*/