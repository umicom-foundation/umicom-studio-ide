/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/designer_window.c
 *
 * PURPOSE:
 *   Compose Framework designer capabilities into the Studio GTK4 visual
 *   designer workbench without duplicating reusable designer logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Undo/redo are Studio commands over Framework-owned history. Palette search,
 * geometry and preview policies are also owned by Framework; this file only
 * decides where GTK widgets appear in the Studio window.
 */

#include "designer_window.h"

#include <inttypes.h>

#include "palette_view.h"
#include "tree_view.h"
#include "inspector_view.h"
#include "surface_view.h"
#include "preview_view.h"

typedef struct UmiStudioDesignerWindowState {
    UmiStudioDesigner *designer;
    GtkWidget *status;
} UmiStudioDesignerWindowState;

static void update_status(UmiStudioDesignerWindowState *state,
                          const char *message)
{
    UmiStudioDesignerSnapshot snapshot;
    char text[256];

    if (state == NULL || state->status == NULL) {
        return;
    }

    if (umi_studio_designer_snapshot(state->designer,
                                     &snapshot) != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(state->status),
                           message != NULL ? message : "Designer ready");
        return;
    }

    g_snprintf(text,
               sizeof(text),
               "%s  •  Revision %" PRIu64 "  •  Selection %zu  •  Undo %zu  •  Redo %zu",
               message != NULL ? message : "Framework Designer",
               snapshot.document.revision,
               snapshot.selected_items,
               snapshot.undo_count,
               snapshot.redo_count);
    gtk_label_set_text(GTK_LABEL(state->status), text);
}

static void on_undo_clicked(GtkButton *button, gpointer user_data)
{
    UmiStudioDesignerWindowState *state =
        (UmiStudioDesignerWindowState *)user_data;
    UmiStatus status;

    (void)button;
    status = umi_studio_designer_undo(state->designer);
    update_status(state,
                  status == UMI_STATUS_OK
                      ? "Undo completed"
                      : "Nothing to undo");
}

static void on_redo_clicked(GtkButton *button, gpointer user_data)
{
    UmiStudioDesignerWindowState *state =
        (UmiStudioDesignerWindowState *)user_data;
    UmiStatus status;

    (void)button;
    status = umi_studio_designer_redo(state->designer);
    update_status(state,
                  status == UMI_STATUS_OK
                      ? "Redo completed"
                      : "Nothing to redo");
}

GtkWidget *umi_studio_designer_window_new(
    GtkApplication *application,
    UmiStudioDeclarative *declarative,
    UmiStudioDesigner *designer)
{
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *toolbar;
    GtkWidget *undo_button;
    GtkWidget *redo_button;
    GtkWidget *paned;
    GtkWidget *right;
    GtkWidget *left;
    GtkWidget *bottom;
    UmiStudioDesignerWindowState *state;

    window = gtk_application_window_new(application);
    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    undo_button = gtk_button_new_with_label("Undo");
    redo_button = gtk_button_new_with_label("Redo");
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    right = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    bottom = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

    gtk_window_set_title(GTK_WINDOW(window),
                         "Umicom Studio — Visual Designer");
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 800);

    state = g_new0(UmiStudioDesignerWindowState, 1);
    state->designer = designer;
    state->status = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(state->status), 1.0F);
    gtk_widget_set_hexpand(state->status, TRUE);

    gtk_box_append(GTK_BOX(toolbar), undo_button);
    gtk_box_append(GTK_BOX(toolbar), redo_button);
    gtk_box_append(GTK_BOX(toolbar), state->status);
    gtk_box_append(GTK_BOX(root), toolbar);

    gtk_box_append(GTK_BOX(left),
                   umi_studio_designer_palette_view_new(declarative));
    gtk_box_append(GTK_BOX(left),
                   umi_studio_designer_tree_view_new(designer));

    gtk_paned_set_start_child(GTK_PANED(paned), left);
    gtk_paned_set_end_child(GTK_PANED(paned), right);
    gtk_paned_set_position(GTK_PANED(paned), 300);

    gtk_paned_set_start_child(
        GTK_PANED(right),
        umi_studio_designer_surface_view_new(designer)
    );

    gtk_paned_set_start_child(
        GTK_PANED(bottom),
        umi_studio_designer_inspector_view_new(designer, "root")
    );
    gtk_paned_set_end_child(
        GTK_PANED(bottom),
        umi_studio_designer_preview_view_new(designer)
    );

    gtk_paned_set_end_child(GTK_PANED(right), bottom);
    gtk_paned_set_position(GTK_PANED(right), 520);

    gtk_box_append(GTK_BOX(root), paned);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_window_set_child(GTK_WINDOW(window), root);

    g_object_set_data_full(G_OBJECT(window),
                           "umicom-designer-window-state",
                           state,
                           g_free);

    g_signal_connect(undo_button,
                     "clicked",
                     G_CALLBACK(on_undo_clicked),
                     state);
    g_signal_connect(redo_button,
                     "clicked",
                     G_CALLBACK(on_redo_clicked),
                     state);

    update_status(state, "Framework Designer");
    return window;
}
