/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/menu/include/command_palette.h
 * PURPOSE: Command palette dialog with filterable list of commands (GTK4, C)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMMAND_PALETTE_H
#define UMICOM_COMMAND_PALETTE_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef void (*UmiCommandFn)(gpointer user);

typedef struct {
  const char *name;     /* Button label / action name       */
  const char *hint;     /* Tooltip / subtitle (optional)    */
  UmiCommandFn cb;      /* Callback invoked on activation   */
} UmiCommand;

typedef struct _UmiPalette UmiPalette;

UmiPalette *umi_palette_new (const UmiCommand *table, gpointer user);
void        umi_palette_open(UmiPalette *p, GtkWindow *parent);
void        umi_palette_free(UmiPalette *p);

G_END_DECLS
#endif /* UMICOM_COMMAND_PALETTE_H */
