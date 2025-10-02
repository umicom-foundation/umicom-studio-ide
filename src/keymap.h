/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/keymap.h
 * PURPOSE: Install default accelerators on a window
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KEYMAP_H
#define UMICOM_KEYMAP_H

#include <gtk/gtk.h>

typedef struct {
  gpointer user; /* reserved for future extension */
} UmiKeymapCallbacks;

/* Install default accelerators for core actions. */
void umi_keymap_install(GtkWindow *win, const UmiKeymapCallbacks *cb);

#endif /* UMICOM_KEYMAP_H */
