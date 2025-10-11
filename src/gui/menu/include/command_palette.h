/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/command_palette.h
 * PURPOSE: Command palette dialog with filterable list of commands
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_COMMAND_PALETTE_H
#define UMICOM_COMMAND_PALETTE_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*UmiCommandFn)(gpointer user);

typedef struct {
  const char *name;
  const char *hint;
  UmiCommandFn cb;
} UmiCommand;

typedef struct _UmiPalette UmiPalette;

/* Create palette with a static table of commands (array terminated by name==NULL). */
UmiPalette *umi_palette_new(const UmiCommand *table, gpointer user);
void       umi_palette_open(UmiPalette *p, GtkWindow *parent);
void       umi_palette_free(UmiPalette *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_COMMAND_PALETTE_H */
