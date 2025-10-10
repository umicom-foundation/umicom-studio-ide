/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: theme.c
 * PURPOSE: Theme apply: keep style context but silence unused warning.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include "theme.h"

void umi_theme_apply(GtkWindow *win){
    GtkStyleContext *ctx = gtk_widget_get_style_context(GTK_WIDGET(win));
    (void)ctx;
    /* TODO: load CSS provider and apply */
}
