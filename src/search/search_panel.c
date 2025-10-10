/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: search_panel.c
 * PURPOSE: Search panel: stub signal handler (remove unused var).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>

static void on_clicked(GtkButton *btn, gpointer user_data){
    (void)btn; (void)user_data;
    gchar *out = NULL;
    /* ... real implementation populates 'out' ... */
    g_clear_pointer(&out, g_free);
}
