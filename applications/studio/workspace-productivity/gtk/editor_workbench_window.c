/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/gtk/editor_workbench_window.c
 *
 * PURPOSE:
 *   Build a GTK4 source-editor productivity workbench with open editors, quick-open, find/replace and status UI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include "editor_workbench_window.h"

#include <stdio.h>

#if defined(UMICOM_STUDIO_HAS_SOURCEVIEW)
#include <gtksourceview/gtksource.h>
#endif

#include "umicom/studio/editor_status.h"

typedef struct WorkbenchUi {
    GtkWidget *editor;
    GtkWidget *find_entry;
    GtkWidget *status;
    GtkWidget *quick_open;
} WorkbenchUi;

static GtkWidget *make_editor(void)
{
#if defined(UMICOM_STUDIO_HAS_SOURCEVIEW)
    GtkSourceBuffer *buffer = gtk_source_buffer_new(NULL);
    GtkWidget *view = gtk_source_view_new_with_buffer(buffer);
    GtkSourceLanguageManager *languages =
        gtk_source_language_manager_get_default();
    GtkSourceLanguage *language =
        gtk_source_language_manager_get_language(languages, "c");

    if (language != NULL) {
        gtk_source_buffer_set_language(buffer, language);
        gtk_source_buffer_set_highlight_syntax(buffer, TRUE);
    }

    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(view), TRUE);
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(view), 4U);
    gtk_text_buffer_set_text(
        GTK_TEXT_BUFFER(buffer),
        "/* Umicom Studio workspace productivity */\n"
        "#include <stdio.h>\n\n"
        "int main(void)\n"
        "{\n"
        "    puts(\"Quick Open, sessions, navigation and find/replace are ready.\");\n"
        "    return 0;\n"
        "}\n",
        -1
    );
    g_object_unref(buffer);
    return view;
#else
    GtkWidget *view = gtk_text_view_new();
    gtk_text_buffer_set_text(
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)),
        "GtkSourceView 5 was not found at build time.\n"
        "The workspace productivity model is still available headlessly.\n",
        -1
    );
    return view;
#endif
}

static void update_status(WorkbenchUi *ui)
{
    UmiStudioEditorStatus status;
    char text[256];
    GtkTextIter insert_iter;
    GtkTextBuffer *buffer;

    if (ui == NULL || ui->editor == NULL) {
        return;
    }

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->editor));
    gtk_text_buffer_get_iter_at_mark(
        buffer,
        &insert_iter,
        gtk_text_buffer_get_insert(buffer)
    );

    umi_studio_editor_status_init(&status);
    status.line = (size_t)gtk_text_iter_get_line(&insert_iter) + 1U;
    status.column = (size_t)gtk_text_iter_get_line_offset(&insert_iter) + 1U;
    (void)snprintf(status.language, sizeof(status.language), "C");

    if (umi_studio_editor_status_format(&status,
                                        text,
                                        sizeof(text)) == UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), text);
    }
}

static void on_mark_set(GtkTextBuffer *buffer,
                        GtkTextIter *location,
                        GtkTextMark *mark,
                        gpointer user_data)
{
    (void)buffer;
    (void)location;
    (void)mark;
    update_status((WorkbenchUi *)user_data);
}

static void on_find_changed(GtkEditable *editable, gpointer user_data)
{
    WorkbenchUi *ui = (WorkbenchUi *)user_data;
    const char *query = gtk_editable_get_text(editable);
    char text[256];

    (void)snprintf(text,
                   sizeof(text),
                   "Find: %s%s",
                   query != NULL ? query : "",
                   query != NULL && query[0] != '\0'
                       ? "  •  Ctrl+H for replace workflow"
                       : "");
    gtk_label_set_text(GTK_LABEL(ui->status), text);
}

static GtkWidget *build_sidebar(void)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *title = gtk_label_new("OPEN EDITORS");
    GtkWidget *one = gtk_label_new("main.c");
    GtkWidget *two = gtk_label_new("CMakeLists.txt  [preview]");
    GtkWidget *recent = gtk_label_new("RECENT WORKSPACES");
    GtkWidget *workspace = gtk_label_new("Umicom Studio");

    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 10);
    gtk_label_set_xalign(GTK_LABEL(title), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(one), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(two), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(recent), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(workspace), 0.0f);
    gtk_widget_add_css_class(title, "heading");
    gtk_widget_add_css_class(recent, "heading");

    gtk_box_append(GTK_BOX(box), title);
    gtk_box_append(GTK_BOX(box), one);
    gtk_box_append(GTK_BOX(box), two);
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_box_append(GTK_BOX(box), recent);
    gtk_box_append(GTK_BOX(box), workspace);
    return box;
}

GtkWidget *umi_studio_editor_workbench_window_new(GtkApplication *application)
{
    WorkbenchUi *ui;
    GtkWidget *window;
    GtkWidget *root;
    GtkWidget *top;
    GtkWidget *paned;
    GtkWidget *editor_box;
    GtkWidget *scroller;
    GtkWidget *find_bar;
    GtkWidget *replace_entry;
    GtkTextBuffer *buffer;

    if (application == NULL) {
        return NULL;
    }

    ui = g_new0(WorkbenchUi, 1);

    window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window),
                         "Umicom Studio — Workspace & Editor Productivity");
    gtk_window_set_default_size(GTK_WINDOW(window), 1180, 760);
    g_object_set_data_full(G_OBJECT(window), "workspace-ui", ui, g_free);

    root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), root);

    top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(top, 10);
    gtk_widget_set_margin_end(top, 10);
    gtk_widget_set_margin_top(top, 8);
    gtk_widget_set_margin_bottom(top, 8);

    ui->quick_open = gtk_search_entry_new();
    gtk_widget_set_hexpand(ui->quick_open, TRUE);
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(ui->quick_open),
        "Quick Open — fuzzy search files, documents and symbols (Ctrl+P)"
    );
    gtk_box_append(GTK_BOX(top), ui->quick_open);
    gtk_box_append(GTK_BOX(root), top);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_box_append(GTK_BOX(root), paned);
    gtk_paned_set_start_child(GTK_PANED(paned), build_sidebar());

    editor_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_paned_set_end_child(GTK_PANED(paned), editor_box);
    gtk_paned_set_position(GTK_PANED(paned), 240);

    ui->editor = make_editor();
    scroller = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroller, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), ui->editor);
    gtk_box_append(GTK_BOX(editor_box), scroller);

    find_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_margin_start(find_bar, 8);
    gtk_widget_set_margin_end(find_bar, 8);
    gtk_widget_set_margin_top(find_bar, 6);
    gtk_widget_set_margin_bottom(find_bar, 6);

    ui->find_entry = gtk_search_entry_new();
    gtk_search_entry_set_placeholder_text(
        GTK_SEARCH_ENTRY(ui->find_entry),
        "Find"
    );
    replace_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(replace_entry),
        "Replace"
    );
    gtk_widget_set_hexpand(ui->find_entry, TRUE);
    gtk_widget_set_hexpand(replace_entry, TRUE);
    gtk_box_append(GTK_BOX(find_bar), ui->find_entry);
    gtk_box_append(GTK_BOX(find_bar), replace_entry);
    gtk_box_append(GTK_BOX(editor_box), find_bar);

    ui->status = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(ui->status), 1.0f);
    gtk_widget_set_margin_start(ui->status, 8);
    gtk_widget_set_margin_end(ui->status, 8);
    gtk_widget_set_margin_bottom(ui->status, 6);
    gtk_box_append(GTK_BOX(editor_box), ui->status);

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->editor));
    g_signal_connect(buffer, "mark-set", G_CALLBACK(on_mark_set), ui);
    g_signal_connect(ui->find_entry,
                     "search-changed",
                     G_CALLBACK(on_find_changed),
                     ui);

    update_status(ui);
    return window;
}
