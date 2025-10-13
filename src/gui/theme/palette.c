/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/palette.c
 * PURPOSE:
 *   Theme palette dialog/widget logic (colors, CSS, etc.).
 *   Previously exported as umi_palette_* which collided with the command
 *   palette in src/gui/menu/command_palette.c. We now export the theme API
 *   as umi_theme_palette_* to guarantee unique linkage.
 *
 * DESIGN CHOICES:
 *   - Pure C/GTK4.
 *   - Public API: umi_theme_palette_new/open/free (distinct names).
 *   - Internals kept static to limit coupling.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include "include/palette.h"          // Updated header declares umi_theme_palette_*.
#include <gtk/gtk.h>                  // GTK4 APIs.

/* Opaque state for the theme palette. */
typedef struct UmiThemePalette {
    GtkWidget *dialog;                // Dialog widget (or future replacement).
} UmiThemePalette;

/* Construct the theme palette object. */
UmiThemePalette *umi_theme_palette_new(void) {
    UmiThemePalette *p = g_new0(UmiThemePalette, 1);        // Allocate and zero state.
    /* NOTE: GtkDialog is deprecated in GTK4. We’ll migrate to GtkAlertDialog /
     * GtkWindow + custom content in Phase 2. For now we keep it compiling. */
    G_GNUC_BEGIN_IGNORE_DEPRECATIONS
    p->dialog = gtk_dialog_new();
    G_GNUC_END_IGNORE_DEPRECATIONS                            // Create dialog (deprecated).
    return p;                                                // Return allocated state.
}

/* Present/open the theme palette UI. */
void umi_theme_palette_open(UmiThemePalette *p, GtkWindow *parent) {
    g_return_if_fail(p != NULL);                             // Defensive null check.
    gtk_window_set_transient_for(GTK_WINDOW(p->dialog), parent); // Parent relationship.
    gtk_widget_set_visible(p->dialog, TRUE);
    if (GTK_IS_WINDOW(p->dialog)) gtk_window_present(GTK_WINDOW(p->dialog));                              // Show dialog widget.
}

/* Destroy the theme palette object. */
void umi_theme_palette_free(UmiThemePalette *p) {
    if (!p) return;                                          // Handle NULL gracefully.
    if (p->dialog) gtk_widget_unparent(p->dialog);           // Detach if needed.
    g_free(p);                                               // Free container.
}
/* End of src/gui/theme/palette.c */