/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_bar.c
 * PURPOSE: See file-specific notes below
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "../../include/status_bar.h"

/* PURPOSE (status_bar.c):
 * Implements a simple status bar that embeds the status widget. The bar
 * owns the UmiStatus object and exposes it for modules that expect a
 * UmiStatus* (search panel, build system, etc.).
 */

UmiStatusBar *umi_status_bar_new(void) {
  UmiStatusBar *sb = g_new0(UmiStatusBar, 1);
  sb->root  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  sb->inner = umi_status_new(); /* from status_util.h/c */
  GtkWidget *w = umi_status_widget(sb->inner);
  gtk_box_append(GTK_BOX(sb->root), w);
  return sb;
}

GtkWidget *umi_status_bar_widget(UmiStatusBar *sb) {
  return sb ? sb->root : NULL;
}

UmiStatus *umi_status_bar_inner(UmiStatusBar *sb) {
  return sb ? sb->inner : NULL;
}
