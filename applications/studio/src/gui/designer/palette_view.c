/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/palette_view.c
 *
 * PURPOSE:
 *   Present Framework-owned component palette search in GTK4 without
 *   duplicating palette metadata or fuzzy-search logic in Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file owns GTK widgets only. Component metadata, ranking and filtering
 * remain in Umicom Framework so future applications can reuse them.
 */

#include "palette_view.h"

typedef struct UmiStudioPaletteViewState {
    UmiDesignerPalette palette;
    GtkWidget *list;
} UmiStudioPaletteViewState;

static void clear_list(GtkWidget *list)
{
    GtkWidget *child = gtk_widget_get_first_child(list);

    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list), child);
        child = next;
    }
}

static void append_component_row(
    GtkWidget *list,
    const UmiDeclComponentDescriptor *component)
{
    GtkWidget *box;
    GtkWidget *name;
    GtkWidget *meta;
    char detail[320];

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);

    name = gtk_label_new(component->display_name);
    gtk_label_set_xalign(GTK_LABEL(name), 0.0F);

    g_snprintf(detail,
               sizeof(detail),
               "%s  •  %s%s",
               component->component_type,
               component->category,
               component->container ? "  •  container" : "");
    meta = gtk_label_new(detail);
    gtk_label_set_xalign(GTK_LABEL(meta), 0.0F);
    gtk_widget_add_css_class(meta, "dim-label");

    gtk_box_append(GTK_BOX(box), name);
    gtk_box_append(GTK_BOX(box), meta);
    gtk_list_box_append(GTK_LIST_BOX(list), box);
}

static void rebuild(
    UmiStudioPaletteViewState *state,
    const char *query)
{
    UmiDesignerPaletteResults results;
    size_t index;

    clear_list(state->list);

    if (query == NULL || query[0] == '\0') {
        for (index = 0U; index < state->palette.count; ++index) {
            append_component_row(state->list, &state->palette.items[index]);
        }
        return;
    }

    if (umi_designer_palette_search(&state->palette,
                                    query,
                                    &results) != UMI_STATUS_OK) {
        return;
    }

    for (index = 0U; index < results.count; ++index) {
        const UmiDeclComponentDescriptor *component =
            umi_designer_palette_result(&state->palette, &results, index);
        if (component != NULL) {
            append_component_row(state->list, component);
        }
    }
}

static void on_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
    UmiStudioPaletteViewState *state =
        (UmiStudioPaletteViewState *)user_data;

    rebuild(state, gtk_editable_get_text(GTK_EDITABLE(entry)));
}

static void on_state_free(gpointer data)
{
    UmiStudioPaletteViewState *state =
        (UmiStudioPaletteViewState *)data;

    if (state != NULL) {
        umi_designer_palette_dispose(&state->palette);
        g_free(state);
    }
}

GtkWidget *umi_studio_designer_palette_view_new(
    UmiStudioDeclarative *declarative)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *title = gtk_label_new("Components");
    GtkWidget *search = gtk_search_entry_new();
    GtkWidget *scroll = gtk_scrolled_window_new();
    UmiStudioPaletteViewState *state;

    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "heading");
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(search),
        "Search Framework components"
    );

    gtk_box_append(GTK_BOX(root), title);
    gtk_box_append(GTK_BOX(root), search);
    gtk_box_append(GTK_BOX(root), scroll);
    gtk_widget_set_size_request(root, 260, -1);

    state = g_new0(UmiStudioPaletteViewState, 1);
    state->list = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->list);
    gtk_widget_set_vexpand(scroll, TRUE);

    if (declarative != NULL &&
        umi_designer_palette_build(
            umi_studio_declarative_components(declarative),
            NULL,
            &state->palette) == UMI_STATUS_OK) {
        rebuild(state, "");
    }

    g_object_set_data_full(G_OBJECT(root),
                           "umicom-palette-state",
                           state,
                           on_state_free);
    g_signal_connect(search,
                     "search-changed",
                     G_CALLBACK(on_search_changed),
                     state);

    return root;
}
