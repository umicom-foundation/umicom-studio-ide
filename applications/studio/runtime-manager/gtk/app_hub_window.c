/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/gtk/app_hub_window.c
 *
 * PURPOSE:
 *   Build the GTK4 Application Hub with search, category views, application cards, details and launch planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include "app_hub_window.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/runtime_catalogue.h"
#include "umicom/studio/runtime_filter.h"
#include "umicom/studio/runtime_launch_plan.h"
#include "umicom/studio/runtime_snapshot.h"

typedef struct AppHubWindow {
    UmiStudioRuntimeManager manager;
    UmiStudioRuntimeFilter filter;
    GtkWidget *list_box;
    GtkWidget *search;
    GtkWidget *category_dropdown;
    GtkWidget *summary_label;
    GtkWidget *detail_name;
    GtkWidget *detail_id;
    GtkWidget *detail_category;
    GtkWidget *detail_state;
    GtkWidget *detail_description;
    GtkWidget *detail_capabilities;
    GtkWidget *plan_result;
    char selected_id[UMI_INTEGRATION_ID_CAPACITY];
} AppHubWindow;

static void refresh_list(AppHubWindow *hub);

static void set_label(GtkWidget *widget, const char *text)
{
    if (widget != NULL) {
        gtk_label_set_text(GTK_LABEL(widget), text != NULL ? text : "");
    }
}

static GtkWidget *make_title_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_widget_add_css_class(label, "title-3");
    return label;
}

static GtkWidget *make_meta_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_add_css_class(label, "dim-label");
    return label;
}

static void clear_list_box(GtkWidget *list_box)
{
    GtkWidget *child;

    if (list_box == NULL) {
        return;
    }

    child = gtk_widget_get_first_child(list_box);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(list_box), child);
        child = next;
    }
}

static void update_summary(AppHubWindow *hub)
{
    UmiStudioRuntimeSnapshot snapshot;
    char text[256];

    if (umi_studio_runtime_snapshot_build(&hub->manager, &snapshot) !=
        UMI_STATUS_OK) {
        return;
    }

    (void)snprintf(text,
                   sizeof(text),
                   "%zu applications  •  %zu installed  •  %zu running  •  %zu favourites",
                   snapshot.total,
                   snapshot.installed,
                   snapshot.running,
                   snapshot.favourites);
    set_label(hub->summary_label, text);
}

static void update_details(AppHubWindow *hub,
                           const UmiStudioRuntimeEntry *entry)
{
    char state_text[128];
    char capabilities[1024];
    size_t used = 0U;
    size_t index;
    UmiIntegrationApplicationState state;

    if (hub == NULL || entry == NULL) {
        return;
    }

    (void)snprintf(hub->selected_id,
                   sizeof(hub->selected_id),
                   "%s",
                   entry->application.id);

    state = umi_studio_runtime_manager_state(&hub->manager,
                                             entry->application.id);

    set_label(hub->detail_name, entry->application.name);
    set_label(hub->detail_id, entry->application.id);
    set_label(hub->detail_category,
              umi_studio_runtime_category_text(entry->category));

    (void)snprintf(state_text,
                   sizeof(state_text),
                   "%s%s%s",
                   umi_integration_application_state_text(state),
                   entry->installed ? " • installed" : "",
                   entry->favourite ? " • favourite" : "");
    set_label(hub->detail_state, state_text);
    set_label(hub->detail_description, entry->description);

    capabilities[0] = '\0';
    for (index = 0U; index < entry->application.capability_count; ++index) {
        int written = snprintf(capabilities + used,
                               sizeof(capabilities) - used,
                               "%s%s",
                               index == 0U ? "" : "\n",
                               entry->application.capabilities[index]);
        if (written < 0 ||
            (size_t)written >= sizeof(capabilities) - used) {
            break;
        }
        used += (size_t)written;
    }
    set_label(hub->detail_capabilities,
              used > 0U ? capabilities : "No published capabilities");
    set_label(hub->plan_result, "Select Plan Launch to inspect readiness.");
}

static void on_row_activated(GtkListBox *box,
                             GtkListBoxRow *row,
                             gpointer user_data)
{
    AppHubWindow *hub = (AppHubWindow *)user_data;
    const char *application_id;
    const UmiStudioRuntimeEntry *entry;

    (void)box;

    application_id = g_object_get_data(G_OBJECT(row), "umicom-application-id");
    if (application_id == NULL) {
        return;
    }

    entry = umi_studio_runtime_manager_find_const(&hub->manager,
                                                  application_id);
    update_details(hub, entry);
}

static GtkWidget *make_application_row(AppHubWindow *hub,
                                       const UmiStudioRuntimeEntry *entry)
{
    GtkWidget *row;
    GtkWidget *box;
    GtkWidget *text_box;
    GtkWidget *name;
    GtkWidget *meta;
    GtkWidget *favourite;
    char meta_text[256];
    UmiIntegrationApplicationState state;

    row = gtk_list_box_row_new();
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);

    text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    name = gtk_label_new(entry->application.name);
    gtk_label_set_xalign(GTK_LABEL(name), 0.0f);
    gtk_widget_add_css_class(name, "heading");

    state = umi_studio_runtime_manager_state(&hub->manager,
                                             entry->application.id);
    (void)snprintf(meta_text,
                   sizeof(meta_text),
                   "%s • %s • %zu capabilities",
                   umi_studio_runtime_category_text(entry->category),
                   umi_integration_application_state_text(state),
                   entry->application.capability_count);

    meta = gtk_label_new(meta_text);
    gtk_label_set_xalign(GTK_LABEL(meta), 0.0f);
    gtk_widget_add_css_class(meta, "dim-label");

    favourite = gtk_label_new(entry->favourite ? "★" : "");
    gtk_widget_set_halign(favourite, GTK_ALIGN_END);

    gtk_box_append(GTK_BOX(text_box), name);
    gtk_box_append(GTK_BOX(text_box), meta);
    gtk_widget_set_hexpand(text_box, TRUE);
    gtk_box_append(GTK_BOX(box), text_box);
    gtk_box_append(GTK_BOX(box), favourite);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);

    g_object_set_data_full(G_OBJECT(row),
                           "umicom-application-id",
                           g_strdup(entry->application.id),
                           g_free);
    return row;
}

static void refresh_list(AppHubWindow *hub)
{
    size_t index;
    const char *search_text;
    guint selected_category;

    clear_list_box(hub->list_box);

    search_text = gtk_editable_get_text(GTK_EDITABLE(hub->search));
    (void)snprintf(hub->filter.query,
                   sizeof(hub->filter.query),
                   "%s",
                   search_text != NULL ? search_text : "");

    selected_category =
        gtk_drop_down_get_selected(GTK_DROP_DOWN(hub->category_dropdown));
    hub->filter.category =
        selected_category < (guint)UMI_STUDIO_RUNTIME_CATEGORY_COUNT
            ? (UmiStudioRuntimeCategory)selected_category
            : UMI_STUDIO_RUNTIME_CATEGORY_ALL;

    for (index = 0U; index < hub->manager.count; ++index) {
        const UmiStudioRuntimeEntry *entry = &hub->manager.entries[index];
        UmiIntegrationApplicationState state =
            umi_studio_runtime_manager_state(&hub->manager,
                                             entry->application.id);

        if (umi_studio_runtime_filter_match(&hub->filter, entry, state)) {
            gtk_list_box_append(GTK_LIST_BOX(hub->list_box),
                                make_application_row(hub, entry));
        }
    }

    update_summary(hub);
}

static void on_filter_changed(GtkWidget *widget, gpointer user_data)
{
    (void)widget;
    refresh_list((AppHubWindow *)user_data);
}

static void on_favourite_clicked(GtkButton *button, gpointer user_data)
{
    AppHubWindow *hub = (AppHubWindow *)user_data;
    UmiStudioRuntimeEntry *entry;

    (void)button;

    if (hub->selected_id[0] == '\0') {
        set_label(hub->plan_result, "Select an application first.");
        return;
    }

    entry = umi_studio_runtime_manager_find(&hub->manager, hub->selected_id);
    if (entry == NULL) {
        return;
    }

    (void)umi_studio_runtime_manager_set_favourite(&hub->manager,
                                                    hub->selected_id,
                                                    !entry->favourite);
    update_details(hub, entry);
    refresh_list(hub);
}

static void on_plan_clicked(GtkButton *button, gpointer user_data)
{
    AppHubWindow *hub = (AppHubWindow *)user_data;
    UmiStudioRuntimeLaunchPlan plan;
    char text[1024];
    UmiStatus status;

    (void)button;

    if (hub->selected_id[0] == '\0') {
        set_label(hub->plan_result, "Select an application first.");
        return;
    }

    status = umi_studio_runtime_plan_launch(&hub->manager,
                                            hub->selected_id,
                                            &plan);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
        set_label(hub->plan_result, "Unable to build launch plan.");
        return;
    }

    (void)snprintf(text,
                   sizeof(text),
                   "Readiness: %s\nExecutable: %s\n%s",
                   umi_studio_runtime_launch_readiness_text(plan.readiness),
                   plan.executable[0] != '\0' ? plan.executable : "(not resolved)",
                   plan.reason);
    set_label(hub->plan_result, text);
}

static GtkWidget *build_details(AppHubWindow *hub)
{
    GtkWidget *box;
    GtkWidget *buttons;
    GtkWidget *favourite_button;
    GtkWidget *plan_button;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(box, 18);
    gtk_widget_set_margin_end(box, 18);
    gtk_widget_set_margin_top(box, 18);
    gtk_widget_set_margin_bottom(box, 18);

    hub->detail_name = make_title_label("Select an application");
    hub->detail_id = make_meta_label("");
    hub->detail_category = make_meta_label("");
    hub->detail_state = make_meta_label("");
    hub->detail_description = make_meta_label("");
    hub->detail_capabilities = make_meta_label("");
    hub->plan_result = make_meta_label("Select an application to inspect runtime details.");

    gtk_box_append(GTK_BOX(box), hub->detail_name);
    gtk_box_append(GTK_BOX(box), hub->detail_id);
    gtk_box_append(GTK_BOX(box), hub->detail_category);
    gtk_box_append(GTK_BOX(box), hub->detail_state);
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(box), hub->detail_description);
    gtk_box_append(GTK_BOX(box), make_title_label("Capabilities"));
    gtk_box_append(GTK_BOX(box), hub->detail_capabilities);

    buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    favourite_button = gtk_button_new_with_label("Toggle Favourite");
    plan_button = gtk_button_new_with_label("Plan Launch");
    gtk_box_append(GTK_BOX(buttons), favourite_button);
    gtk_box_append(GTK_BOX(buttons), plan_button);
    gtk_box_append(GTK_BOX(box), buttons);

    gtk_box_append(GTK_BOX(box), make_title_label("Launch Plan"));
    gtk_box_append(GTK_BOX(box), hub->plan_result);

    g_signal_connect(favourite_button,
                     "clicked",
                     G_CALLBACK(on_favourite_clicked),
                     hub);
    g_signal_connect(plan_button,
                     "clicked",
                     G_CALLBACK(on_plan_clicked),
                     hub);

    return box;
}

GtkWidget *umi_studio_app_hub_window_new(GtkApplication *application)
{
    AppHubWindow *hub;
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *toolbar;
    GtkWidget *content;
    GtkWidget *left;
    GtkWidget *scroller;
    GtkStringList *categories;
    const char *category_names[] = {
        "All",
        "Development",
        "Finance",
        "AI",
        "Creator",
        "System",
        "Service",
        NULL
    };

    if (application == NULL) {
        return NULL;
    }

    hub = g_new0(AppHubWindow, 1);
    umi_studio_runtime_manager_init(&hub->manager);
    umi_studio_runtime_filter_init(&hub->filter);

    if (umi_studio_runtime_catalogue_populate(&hub->manager) != UMI_STATUS_OK) {
        g_free(hub);
        return NULL;
    }

    window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Umicom Studio — Application Hub");
    gtk_window_set_default_size(GTK_WINDOW(window), 1120, 720);
    g_object_set_data_full(G_OBJECT(window), "umicom-app-hub", hub, g_free);

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), root);

    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(toolbar, 12);
    gtk_widget_set_margin_end(toolbar, 12);
    gtk_widget_set_margin_top(toolbar, 10);
    gtk_widget_set_margin_bottom(toolbar, 10);

    hub->search = gtk_search_entry_new();
    gtk_widget_set_hexpand(hub->search, TRUE);
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(hub->search),
        "Search applications, capabilities or categories"
    );

    categories = gtk_string_list_new(category_names);
    hub->category_dropdown =
        gtk_drop_down_new(G_LIST_MODEL(categories), NULL);
    g_object_unref(categories);

    hub->summary_label = make_meta_label("");

    gtk_box_append(GTK_BOX(toolbar), hub->search);
    gtk_box_append(GTK_BOX(toolbar), hub->category_dropdown);
    gtk_box_append(GTK_BOX(root), toolbar);
    gtk_box_append(GTK_BOX(root), hub->summary_label);

    content = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(content, TRUE);
    gtk_box_append(GTK_BOX(root), content);

    left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    hub->list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(hub->list_box),
                                    GTK_SELECTION_SINGLE);

    scroller = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller),
                                  hub->list_box);
    gtk_widget_set_vexpand(scroller, TRUE);
    gtk_box_append(GTK_BOX(left), scroller);

    gtk_paned_set_start_child(GTK_PANED(content), left);
    gtk_paned_set_end_child(GTK_PANED(content), build_details(hub));
    gtk_paned_set_position(GTK_PANED(content), 500);

    g_signal_connect(hub->list_box,
                     "row-activated",
                     G_CALLBACK(on_row_activated),
                     hub);
    g_signal_connect(hub->search,
                     "search-changed",
                     G_CALLBACK(on_filter_changed),
                     hub);
    g_signal_connect(hub->category_dropdown,
                     "notify::selected",
                     G_CALLBACK(on_filter_changed),
                     hub);

    refresh_list(hub);
    return window;
}
