/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/palette.h
 * PURPOSE: Simple command palette stub (search + execute callbacks)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PALETTE_H
#define UMICOM_PALETTE_H

#include <gtk/gtk.h>
#include "../../../util/sys/include/utf8.h"

typedef void (*UmiCmdFn)(gpointer);

typedef struct {
  const char *name;
  UmiCmdFn    fn;
  gpointer    user;
} UmiCmd;

typedef struct {
  GtkWidget *dialog;
  GtkEntry  *entry;
  GtkListBox *list;
  GPtrArray *cmds; /* of UmiCmd* (not owned; demo stub) */
} UmiPalette;

UmiPalette *umi_palette_new(GtkWindow *parent);
void        umi_palette_set_commands(UmiPalette *p, GPtrArray *cmds);
void        umi_palette_open(UmiPalette *p);

#endif /* UMICOM_PALETTE_H */
