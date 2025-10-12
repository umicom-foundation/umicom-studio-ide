/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/include/palette.h
 * PURPOSE: Simple command palette stub (search + execute callbacks)
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab
 * Date: 2025-10-01
 * License: MIT
 *
 * Notes on design (loose coupling):
 *  - This header depends only on GTK and GLib. No cross-folder project
 *    headers are required.
 *  - The UTF-8 search helper is implemented privately in palette.c, so we
 *    do NOT include any external "utf8.h" utilities here.
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_THEME_PALETTE_H
#define UMICOM_THEME_PALETTE_H

#include <gtk/gtk.h>

/* Callback type for palette commands. The gpointer is app-defined context. */
typedef void (*UmiCmdFn)(gpointer user);

/* A single command entry shown in the palette. */
typedef struct {
  const char *name;   /* Display name; not owned by the palette.            */
  UmiCmdFn    fn;     /* Action to invoke when the item is activated.       */
  gpointer    user;   /* Arbitrary user pointer forwarded to the callback.  */
} UmiCmd;

/* Opaque(ish) controller for the palette dialog. */
typedef struct {
  GtkWidget  *dialog; /* Top-level dialog window.                            */
  GtkEntry   *entry;  /* Search entry.                                       */
  GtkListBox *list;   /* Results list.                                       */
  GPtrArray  *cmds;   /* Array of UmiCmd* (not owned; caller manages items). */
} UmiPalette;

/* Construction & control API */
UmiPalette *umi_palette_new          (GtkWindow *parent);
void        umi_palette_set_commands (UmiPalette *p, GPtrArray *cmds);
void        umi_palette_open         (UmiPalette *p);
void        umi_palette_free         (UmiPalette *p);

#endif /* UMICOM_THEME_PALETTE_H */
/*--- end of file ---*/