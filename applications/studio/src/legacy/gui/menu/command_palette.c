/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/menu/command_palette.c
 * PURPOSE: Minimal Command Palette (GTK4, pure C)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <ctype.h>

#include "command_palette.h"

/* Opaque state (defined only in .c) */
struct _UmiPalette {
  const UmiCommand *table;
  gpointer          user;

  GtkWindow *win;
  GtkWidget *content;
  GtkWidget *entry;
  GtkWidget *list;
};

/* Case-insensitive substring search. */
static const char *tiny_strcasestr(const char *hay, const char *nee)
{
  if (!hay || !nee || !*nee) return hay;
  const size_t nl = strlen(nee);
  for (const char *p = hay; *p; ++p) {
    size_t i = 0;
    while (i < nl) {
      int a = (unsigned char)tolower((unsigned char)p[i]);
      int b = (unsigned char)tolower((unsigned char)nee[i]);
      if (a != b) break;
      ++i;
    }
    if (i == nl) return p;
  }
  return NULL;
}

/* Remove all children from a GtkBox (GTK4). */
static void clear_list(GtkWidget *box)
{
  if (!box) return;
  GtkWidget *child = gtk_widget_get_first_child(box);
  while (child) {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    gtk_box_remove(GTK_BOX(box), child);
    child = next ? next : gtk_widget_get_first_child(box);
  }
}

static void populate(struct _UmiPalette *p, const char *q);

static void on_command_clicked(GtkButton *btn, gpointer user)
{
  struct _UmiPalette *p = (struct _UmiPalette*)user;
  if (!p || !p->table) return;

  gpointer idxp = g_object_get_data(G_OBJECT(btn), "idx");
  const guint idx = (guint)(uintptr_t)idxp;
  const UmiCommand *cmd = &p->table[idx];

  if (cmd && cmd->cb) cmd->cb(p->user);
  gtk_window_close(p->win);
}

static void populate(struct _UmiPalette *p, const char *q)
{
  if (!p || !p->list || !p->table) return;

  clear_list(p->list);

  for (guint i = 0; p->table[i].name; ++i) {
    const char *name = p->table[i].name;
    const char *hint = p->table[i].hint;

    if (q && *q) {
      gboolean match = tiny_strcasestr(name, q) != NULL;
      if (!match && hint) match = tiny_strcasestr(hint, q) != NULL;
      if (!match) continue;
    }

    GtkWidget *btn = gtk_button_new_with_label(name);
    if (hint && *hint) gtk_widget_set_tooltip_text(btn, hint);

    g_object_set_data(G_OBJECT(btn), "idx", (gpointer)(uintptr_t)i);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_command_clicked), p);
    gtk_box_append(GTK_BOX(p->list), btn);
  }
}

static void on_changed(GtkEditable *e, gpointer user)
{
  struct _UmiPalette *p = (struct _UmiPalette*)user;
  if (!p) return;
  populate(p, gtk_editable_get_text(e));
}

UmiPalette *umi_palette_new(const UmiCommand *table, gpointer user)
{
  struct _UmiPalette *p = g_new0(struct _UmiPalette, 1);
  p->table = table;
  p->user  = user;

  p->win = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(p->win, "Command Palette");
  gtk_window_set_default_size(p->win, 600, 420);
  gtk_window_set_modal(p->win, TRUE);

  p->content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_window_set_child(p->win, p->content);

  p->entry = gtk_entry_new();
  gtk_box_append(GTK_BOX(p->content), p->entry);

  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_box_append(GTK_BOX(p->content), scroll);

  p->list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), p->list);

  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);

  populate(p, "");
  return (UmiPalette*)p;
}

void umi_palette_open(UmiPalette *pp, GtkWindow *parent)
{
  struct _UmiPalette *p = (struct _UmiPalette*)pp;
  if (!p) return;
  if (parent) gtk_window_set_transient_for(p->win, parent);
  gtk_window_present(p->win);
}

void umi_palette_free(UmiPalette *pp)
{
  struct _UmiPalette *p = (struct _UmiPalette*)pp;
  if (!p) return;
  gtk_window_destroy(p->win);
  g_free(p);
}
