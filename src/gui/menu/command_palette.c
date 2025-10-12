/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/menu/command_palette.c
 * PURPOSE: Minimal Command Palette (filter + list of actions) for GTK4
 *          Self-contained (pure C), matches command_palette.h.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------
 * Design goals:
 *  - Loosely coupled: only depends on this module's public header.
 *  - Private state lives in this .c (struct UmiPalette), not exposed in headers.
 *  - Clean C (C11) — no C++-style declarations inside conditions.
 *  - GTK4-only APIs; no GtkBuilder, no resources, no deprecated helpers.
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>   /* strlen */
#include <ctype.h>    /* tolower */

/* Prefer module-local public header path */
#if __has_include("include/command_palette.h")
#  include "include/command_palette.h"
#else
#  include "command_palette.h"
#endif

/*-----------------------------------------------------------------------------
 * Private controller state (opaque to users of this module).
 *
 * NOTE: The header declares "typedef struct _UmiPalette UmiPalette;" so the
 *       type is opaque. We define its layout here so we can access fields.
 *---------------------------------------------------------------------------*/
struct _UmiPalette {
  const UmiCommand *table; /* static command table (terminated by name==NULL) */
  gpointer          user;  /* opaque user payload forwarded to callbacks      */

  GtkWindow *win;          /* toplevel window acting as the palette dialog    */
  GtkWidget *content;      /* root vertical box                               */
  GtkWidget *entry;        /* filter entry                                    */
  GtkWidget *list;         /* vertical box with buttons (re-populated)        */
};

/*-----------------------------------------------------------------------------
 * tiny_strcasestr:
 * Case-insensitive substring search. Returns pointer to first match or NULL.
 *---------------------------------------------------------------------------*/
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
    if (i == nl) return p; /* found */
  }
  return NULL; /* not found */
}

/*-----------------------------------------------------------------------------
 * clear_list:
 * Remove all children from the vertical list box (GTK4).
 * IMPORTANT (C): declare variables outside the condition; no C++-style decls.
 *---------------------------------------------------------------------------*/
static void clear_list(GtkWidget *box)
{
  if (!box) return;

  /* Strategy: repeatedly grab the *first* child and remove it. */
  GtkWidget *child = gtk_widget_get_first_child(box);
  while (child) {
    GtkWidget *next = gtk_widget_get_next_sibling(child); /* cache next */
    gtk_box_remove(GTK_BOX(box), child);
    child = next ? next : gtk_widget_get_first_child(box);
  }
}

/* Forward decls */
static void populate(UmiPalette *p, const char *q);

/*-----------------------------------------------------------------------------
 * on_command_clicked:
 * Button handler for each command; invokes the callback and closes the window.
 *---------------------------------------------------------------------------*/
static void on_command_clicked(GtkButton *btn, gpointer user)
{
  (void)btn;
  UmiPalette *p = (UmiPalette*)user;
  if (!p || !p->table || !p->list) return;

  /* Each button carries its table index as "idx". */
  gpointer idxp = g_object_get_data(G_OBJECT(btn), "idx");
  const uintptr_t idx = (uintptr_t)idxp;

  const UmiCommand *cmd = &p->table[idx];
  if (cmd && cmd->cb) {
    cmd->cb(p->user);
  }

  gtk_window_close(p->win);
}

/*-----------------------------------------------------------------------------
 * populate:
 * Rebuild button list using current query "q".
 * - Matches against UmiCommand.name (and optionally .hint).
 *---------------------------------------------------------------------------*/
static void populate(UmiPalette *p, const char *q)
{
  if (!p || !p->list || !p->table) return;

  clear_list(p->list);

  for (guint i = 0; p->table[i].name != NULL; ++i) {
    const char *name = p->table[i].name;
    const char *hint = p->table[i].hint;

    /* Filter: case-insensitive substring match on name (or hint). */
    if (q && *q) {
      gboolean match = tiny_strcasestr(name, q) != NULL;
      if (!match && hint) match = tiny_strcasestr(hint, q) != NULL;
      if (!match) continue;
    }

    GtkWidget *btn = gtk_button_new_with_label(name);

    /* Optional: show hint as tooltip if provided. */
    if (hint && *hint) gtk_widget_set_tooltip_text(btn, hint);

    g_object_set_data(G_OBJECT(btn), "idx", (gpointer)(uintptr_t)i);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_command_clicked), p);

    gtk_box_append(GTK_BOX(p->list), btn);
  }
}

/*-----------------------------------------------------------------------------
 * on_changed:
 * Entry "changed" signal — live filtering as the user types.
 *---------------------------------------------------------------------------*/
static void on_changed(GtkEditable *e, gpointer user)
{
  UmiPalette *p = (UmiPalette*)user;
  if (!p) return;

  const char *q = gtk_editable_get_text(e);
  populate(p, q);
}

/*-----------------------------------------------------------------------------
 * umi_palette_new:
 * Build a modal-like palette window with:
 *   - a filter entry
 *   - a vertical list of matching commands
 *---------------------------------------------------------------------------*/
UmiPalette *umi_palette_new(const UmiCommand *table, gpointer user)
{
  UmiPalette *p = g_new0(UmiPalette, 1);
  p->table = table;
  p->user  = user;

  /* Create a plain window for GTK4 (avoid deprecated GtkDialog helpers). */
  p->win = GTK_WINDOW(gtk_window_new());
  gtk_window_set_title(p->win, "Command Palette");
  gtk_window_set_default_size(p->win, 600, 420);
  gtk_window_set_modal(p->win, TRUE);

  /* Vertical content box. */
  p->content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_window_set_child(p->win, p->content);

  /* Filter entry. */
  p->entry = gtk_entry_new();
  gtk_box_append(GTK_BOX(p->content), p->entry);

  /* Scrollable list area. */
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_box_append(GTK_BOX(p->content), scroll);

  /* Vertical list box inside the scrolled window. */
  p->list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), p->list);

  /* Wire live filtering. */
  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);

  /* Initial population: no filter. */
  populate(p, "");

  return p;
}

/*-----------------------------------------------------------------------------
 * umi_palette_open:
 * Present the window; make it transient for parent if provided.
 *---------------------------------------------------------------------------*/
void umi_palette_open(UmiPalette *p, GtkWindow *parent)
{
  if (!p) return;
  if (parent) gtk_window_set_transient_for(p->win, parent);
  gtk_window_present(p->win);
}

/*-----------------------------------------------------------------------------
 * umi_palette_free:
 * Destroy the window and free the controller.
 *---------------------------------------------------------------------------*/
void umi_palette_free(UmiPalette *p)
{
  if (!p) return;
  gtk_window_destroy(p->win);
  g_free(p);
}
/*--- end of file ---*/