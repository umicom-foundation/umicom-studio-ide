/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/menu/command_palette.c
 * PURPOSE: Minimal Command Palette (filter + list of actions) for GTK4
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/

#include "command_palette.h" /* Declares UmiPalette, UmiCommand table */
#include <string.h>                   /* strstr / strcasestr (platform dep)    */
#include <ctype.h>                    /* tolower for case-insensitive match    */

/*-----------------------------------------------------------------------------
 * tiny_strcasestr:
 * A small, portable case-insensitive substring search.
 * GTK/GLib has g_strstr_len but not always case-insensitive across platforms,
 * so we provide a minimal helper. Returns pointer to first match or NULL.
 *---------------------------------------------------------------------------*/
static const char *tiny_strcasestr(const char *hay, const char *nee){
  if(!hay || !nee || !*nee) return hay;
  size_t nl = strlen(nee);
  for(const char *p = hay; *p; ++p){
    size_t i = 0;
    while(i < nl){
      int a = (unsigned char)tolower((unsigned char)p[i]);
      int b = (unsigned char)tolower((unsigned char)nee[i]);
      if(a != b) break;
      ++i;
    }
    if(i == nl) return p; /* found */
  }
  return NULL; /* not found */
}

/*-----------------------------------------------------------------------------
 * clear_list:
 * Utility to remove all children from the vertical container that holds
 * the command buttons. This keeps the palette in sync with the current query.
 *---------------------------------------------------------------------------*/
static void clear_list(GtkWidget *box){
  if(!box) return;
  /* Remove children one by one. */
  while(GtkWidget *child = gtk_widget_get_first_child(box)){
    gtk_box_remove(GTK_BOX(box), child);
  }
}

/*-----------------------------------------------------------------------------
 * on_command_clicked:
 * Handler used for each command button. It calls the action function from the
 * command table, passing through the user pointer, then closes the dialog.
 *---------------------------------------------------------------------------*/
static void on_command_clicked(GtkButton *btn, gpointer user){
  (void)btn; /* not used directly; silence -Wunused-parameter */
  UmiPalette *p = (UmiPalette*)user;
  if(!p || !p->table || !p->list) return;

  /* Each button stores its table index as "idx" using GTK's object data.     */
  gpointer idxp = g_object_get_data(G_OBJECT(btn), "idx");
  const uintptr_t idx = (uintptr_t)idxp;

  const UmiCommand *cmd = &p->table[idx];
  if(cmd && cmd->action){
    /* Execute action and close the palette afterwards.                       */
    cmd->action(p->user);
  }

  /* Close the dialog after executing the command to mimic modal palettes.    */
  gtk_window_close(GTK_WINDOW(p->dlg));
}

/*-----------------------------------------------------------------------------
 * populate:
 * Rebuild the button list filtered by query @q.
 * Filter rule: case-insensitive substring on the command title.
 *---------------------------------------------------------------------------*/
static void populate(UmiPalette *p, const char *q){
  if(!p || !p->list || !p->table) return;

  clear_list(p->list);

  /* Iterate command table until we hit a NULL title sentinel.                */
  for(guint i = 0; p->table[i].title != NULL; ++i){
    const char *title = p->table[i].title;

    /* If there's a query, only show matches; otherwise show all.             */
    if(q && *q){
      if(!tiny_strcasestr(title, q))
        continue; /* not a match */
    }

    /* Create a button per command; label is the command title.               */
    GtkWidget *btn = gtk_button_new_with_label(title);

    /* Store the table index on the button so the click handler can resolve.  */
    g_object_set_data(G_OBJECT(btn), "idx", (gpointer)(uintptr_t)i);

    /* Connect click to invoke the action and close.                           */
    g_signal_connect(btn, "clicked",
                     G_CALLBACK(on_command_clicked), p);

    /* Pack into the vertical box (GTK4 uses gtk_box_append).                 */
    gtk_box_append(GTK_BOX(p->list), btn);
  }
}

/*-----------------------------------------------------------------------------
 * on_changed:
 * Entry "changed" signal. Refines the list live as the user types.
 *---------------------------------------------------------------------------*/
static void on_changed(GtkEditable *e, gpointer user){
  UmiPalette *p = (UmiPalette*)user;
  if(!p) return;

  /* Pull the current text from the entry (owned by GTK; do not free).        */
  const char *q = gtk_editable_get_text(e);

  /* Rebuild the visible list to match the query.                              */
  populate(p, q);
}

/*-----------------------------------------------------------------------------
 * umi_palette_new:
 * Construct a Command Palette controller+dialog. The palette is modal-like:
 * it owns a small dialog containing a filter entry and a vertical list of
 * matching commands from the supplied table.
 * Parameters:
 *   - table: array of UmiCommand entries ending with { NULL, NULL }
 *   - user:  opaque pointer forwarded to each command's action callback
 * Returns:
 *   - Newly allocated UmiPalette*; free with umi_palette_free().
 *---------------------------------------------------------------------------*/
UmiPalette *umi_palette_new(const UmiCommand *table, gpointer user){
  /* Allocate the controller struct with zeros for safety.                    */
  UmiPalette *p = g_new0(UmiPalette, 1);
  p->table = table;            /* remember the command table we filter       */
  p->user  = user;             /* forward this to command callbacks         */

  /* Build a simple dialog shell and set its window title.                    */
  p->dlg = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(p->dlg), "Command Palette");

  /* Grab the content box of the dialog; it’s where we add our widgets.       */
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(p->dlg));

  /* Create the text entry used for filtering and a vertical list container.  */
  p->entry = gtk_entry_new();
  p->list  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4 /* spacing */);

  /* Pack entry then list into the dialog content box.                        */
  gtk_box_append(GTK_BOX(content), p->entry);
  gtk_box_append(GTK_BOX(content), p->list);

  /* Live-filter the list whenever the entry changes.                         */
  g_signal_connect(p->entry, "changed", G_CALLBACK(on_changed), p);

  /* Initial population: show all commands with an empty query.               */
  populate(p, "");

  return p;
}

/*-----------------------------------------------------------------------------
 * umi_palette_open:
 * Presents the palette dialog. If a parent window is provided we mark the
 * dialog transient for nicer stacking/focus behavior on some desktops.
 *---------------------------------------------------------------------------*/
void umi_palette_open(UmiPalette *p, GtkWindow *parent){
  if(!p) return;

  /* Optional: make the dialog transient for parent (no-op if parent NULL).   */
  if(parent)
    gtk_window_set_transient_for(GTK_WINDOW(p->dlg), parent);

  /* Present the dialog (GTK4 replaces gtk_widget_show_all).                  */
  gtk_window_present(GTK_WINDOW(p->dlg));
}

/*-----------------------------------------------------------------------------
 * umi_palette_free:
 * Destroys the dialog and releases the controller struct.
 * Safe to call with NULL; subsequent use is undefined.
 *---------------------------------------------------------------------------*/
void umi_palette_free(UmiPalette *p){
  if(!p) return;

  /* Destroy GTK toplevel owned by this controller.                           */
  gtk_window_destroy(GTK_WINDOW(p->dlg));

  /* Free the controller itself.                                              */
  g_free(p);
}
