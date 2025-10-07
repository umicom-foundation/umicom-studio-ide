/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/status_bar.h
 * PURPOSE: See file-specific notes below
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#pragma once
#include <gtk/gtk.h>
#include "status_util.h"

/* PURPOSE (status_bar.h):
 * Thin wrapper that presents a horizontal status "bar" containing the basic
 * UmiStatus widget provided by status_util.h. This lets code that needs a
 * UmiStatus* access the inner status object (via umi_status_bar_inner())
 * while higher-level UI can keep the bar container widget.
 */

typedef struct _UmiStatusBar {
  GtkWidget *root;   /* container widget shown in the UI */
  UmiStatus *inner;  /* owned inner status primitive */
} UmiStatusBar;

UmiStatusBar *umi_status_bar_new(void);
GtkWidget    *umi_status_bar_widget(UmiStatusBar *sb);
UmiStatus    *umi_status_bar_inner(UmiStatusBar *sb);
