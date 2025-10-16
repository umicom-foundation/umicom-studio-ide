/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/window.c
 *
 * PURPOSE:
 *   Build the main application window in *pure C* with GTK4 primitives.
 *   The layout is opinionated for an IDE:
 *     [ File Tree ] | [ Editor (top) over [ Output | Problems ] (bottom) ] | [ Chat ]
 *   - The editor column starts WIDE (so it's the star).
 *   - The chat pane is optional and toggleable (action: app.toggle-chat).
 *   - Everything uses GtkPaned for easy resizing by users.
 *
 * DESIGN GOALS:
 *   - Zero GtkBuilder/XML (keeps things obvious to new C programmers).
 *   - Loosely coupled: we only expose a single factory function and rely on
 *     tiny helpers (icons/splash/chat mount) that are optional.
 *   - GTK4-only, no deprecated API calls.
 *
 * HOW TO READ THIS FILE (BEGINNERS):
 *   - Comments explain *every* line. Treat it like a guided tour of GTK.
 *   - If you’re brand new: skim once, then jump into the code sections.
 *
 * API:
 *   GtkWidget *window_new(GtkApplication *app);
 *     -> returns a GtkApplicationWindow already containing the full layout.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>                     /* core GTK4 widgets and helpers      */
#include "window_chat_integration.h"     /* mounts app.toggle-chat action      */
/* Use the short, public header name per project rules (no cross-module paths). */
#include "icon.h"                        /* small helper to show logo in UI    */
#include "theme.h"  // header lives at src/core/include/theme.h
/* Forward declaration of a tiny helper that builds the right side (editor +
 * output tabs) and hands us the "chat box" widget so we can toggle it later.  */
static GtkWidget *build_workspace_column(GtkWidget **out_chat_box);

/*-----------------------------------------------------------------------------
 * window_new
 *   This is the only symbol exported by this file. Call it from your app
 *   startup code to create the main window ready for presentation.
 *---------------------------------------------------------------------------*/
GtkWidget *window_new(GtkApplication *app)
{
    /* Guard against misuse: GTK windows must be tied to a GtkApplication so
     * they inherit app features (single-instance, actions, icon theme, etc.).  */
    if (!app) return NULL;

    /* Create a top-level window bound to the application. In GTK4 the type is
     * GtkApplicationWindow but we keep `GtkWidget*` here to stay generic.     */
    GtkWidget *win = gtk_application_window_new(app);

    /* Give the window a human-friendly title. This shows in the titlebar.     */
    gtk_window_set_title(GTK_WINDOW(win), "Umicom Studio IDE");

    /* Suggest a reasonable starting size. Users can resize freely afterwards. */
    gtk_window_set_default_size(GTK_WINDOW(win), 1280, 800);

    /* GTK4 defaults to a modern header bar. We attach a small logo Image to
     * the left side for branding (works cross-platform even though GTK4
     * doesn’t support per-window icons like GTK3).                            */
    {
        /* Create a header bar widget and set it as the titlebar of our window.*/
        GtkWidget *hb = gtk_header_bar_new();
        gtk_window_set_titlebar(GTK_WINDOW(win), hb);

        /* Create a horizontal box to place the logo image and maybe future
         * toolbar buttons. Alignment/spacing are kept minimal.                */
        GtkWidget *left_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        gtk_header_bar_pack_start(GTK_HEADER_BAR(hb), left_box);

        /* Ask our icons helper to create a small GtkImage (16–24 px) that
         * shows the Umicom logo. If your PNG/SVG is present on disk it will
         * load that; otherwise a tiny built-in XPM fallback is used.          */
        GtkWidget *logo = umi_icon_image_logo(20 /*px*/);
        if (logo) {
            /* Add a tooltip so new users understand the branding.            */
            gtk_widget_set_tooltip_text(logo, "Umicom Studio IDE");
            /* Pack the logo at the start (left) side of the header bar.      */
            gtk_box_append(GTK_BOX(left_box), logo);
        }

        /* OPTIONAL: you could add Run/Stop/Save buttons to the header here.
         * We keep them in your content area for now to match your earlier UI. */
    }

    /* The body of the window is a horizontal paned layout:
     *  - LEFT:  file tree (narrow by default)
     *  - RIGHT: another horizontal paned: [ workspace ] | [ chat ]
     * The "workspace" is a vertical paned: [ editor ] over [ output tabs ].   */
    {
        /* Outermost paned splits File Tree (left) from the rest (right).     */
        GtkWidget *outer = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        /* Put this paned as the only child of the toplevel window.           */
        gtk_window_set_child(GTK_WINDOW(win), outer);

        /* --- LEFT: a placeholder widget representing the file tree. ------- */
        GtkWidget *file_tree = gtk_label_new("File Tree");
        /* Labels are lightweight; we center the text and add some margin.    */
        gtk_label_set_xalign(GTK_LABEL(file_tree), 0.0f);
        gtk_widget_set_margin_start(file_tree, 16);
        gtk_widget_set_margin_top(file_tree, 12);
        /* Attach as the left/start child of the paned.                        */
        gtk_paned_set_start_child(GTK_PANED(outer), file_tree);

        /* --- RIGHT: another paned that splits workspace from chat. -------- */
        GtkWidget *right_split = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
        gtk_paned_set_end_child(GTK_PANED(outer), right_split);

        /* Build the editor+output column. We also get back the chat box
         * pointer from our helper so we can wire toggling on it.             */
        GtkWidget *chat_box = NULL;
        GtkWidget *workspace = build_workspace_column(&chat_box);
        gtk_paned_set_start_child(GTK_PANED(right_split), workspace);

        /* The chat pane starts visible (you may default it to hidden by
         * changing gtk_widget_set_visible(..., FALSE) below).                */
        if (chat_box) {
            gtk_paned_set_end_child(GTK_PANED(right_split), chat_box);
            gtk_widget_set_visible(chat_box, TRUE);
        }

        /* ---- Initial sizes (very important for “code editor” feel) ------- */
        /* Paned “position” is the pixel split from the start edge.
         * We make the file tree narrow (~240px) so the editor stays wide.    */
        gtk_paned_set_position(GTK_PANED(outer),       240);   /* tree width  */
        /* We also set the split between workspace and chat; leave chat small. */
        gtk_paned_set_position(GTK_PANED(right_split), 960);   /* editor span */

        /* Finally, mount the chat toggle action at the app scope so you can
         * bind it to a menu item or key (e.g., Ctrl+Shift+C):                 */
        ustudio_chat_mount_and_actions(app, GTK_WINDOW(win), chat_box);
    }

    /* Return the fully constructed window to the caller (the app).           */
    return win;
}


/*-----------------------------------------------------------------------------
 * build_workspace_column
 *   Constructs a vertical paned widget:
 *       [ editor notebook (top, grows) ]
 *       [ output tabs (bottom, smaller) ]
 *   It also builds a small "chat pane" box (right column) and returns it to
 *   the caller via `out_chat_box`. That box is where the chat UI would live.
 *---------------------------------------------------------------------------*/
static GtkWidget *build_workspace_column(GtkWidget **out_chat_box)
{
    /* Defensive programming: allow NULL; we only write if provided.          */
    if (out_chat_box) *out_chat_box = NULL;

    /* Create a vertical paned widget. In GTK4, GtkPaned uses a “position”
     * measured in pixels to split between its two children.                  */
    GtkWidget *vsplit = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

    /* ------------------ Editor (top) -------------------------------------- */
    /* For now we show a simple multi-tab editor scaffold using GtkNotebook.
     * You likely have a real editor widget (GtkSourceView or your own).
     * Place that widget inside this notebook later.                          */
    GtkWidget *editor_tabs = gtk_notebook_new();
    /* Add one starter tab named "untitled" with a plain GtkTextView.         */
    {
        GtkWidget *text = gtk_text_view_new();
        /* Make the text area expand to fill available space.                 */
        gtk_widget_set_hexpand(text, TRUE);
        gtk_widget_set_vexpand(text, TRUE);
        /* Wrap it in a scrolled window so long files are scrollable.         */
        GtkWidget *scroller = gtk_scrolled_window_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), text);
        /* Add as page 0 with label “untitled”.                               */
        gtk_notebook_append_page(GTK_NOTEBOOK(editor_tabs), scroller,
                                 gtk_label_new("untitled"));
    }
    /* Put the editor notebook as the *top/start* child of the vertical split.*/
    gtk_paned_set_start_child(GTK_PANED(vsplit), editor_tabs);

    /* ------------------ Output tabs (bottom) ------------------------------- */
    /* Bottom area groups “Output” and “Problems” using another notebook.     */
    GtkWidget *bottom_tabs = gtk_notebook_new();
    {
        /* Output console placeholder (use your real output console widget).  */
        GtkWidget *output_view = gtk_text_view_new();
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view)),
                                 "[USIDE] Output pane is ready.\n", -1);
        GtkWidget *out_scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(out_scroll), output_view);
        gtk_widget_set_hexpand(out_scroll, TRUE);
        gtk_widget_set_vexpand(out_scroll, TRUE);
        gtk_notebook_append_page(GTK_NOTEBOOK(bottom_tabs), out_scroll,
                                 gtk_label_new("Output"));

        /* Problems list placeholder (later: hook your diagnostics model).    */
        GtkWidget *problems_view = gtk_text_view_new();
        GtkWidget *pr_scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(pr_scroll), problems_view);
        gtk_widget_set_hexpand(pr_scroll, TRUE);
        gtk_widget_set_vexpand(pr_scroll, TRUE);
        gtk_notebook_append_page(GTK_NOTEBOOK(bottom_tabs), pr_scroll,
                                 gtk_label_new("Problems"));
    }
    /* Attach bottom tabs as the *end* child of the vertical split.           */
    gtk_paned_set_end_child(GTK_PANED(vsplit), bottom_tabs);

    /* Give the editor ~75% of the vertical space initially.                  */
    gtk_paned_set_position(GTK_PANED(vsplit), 600);

    /* ------------------ Optional Chat Pane (right column) ------------------ */
    /* We create a plain vertical box that will live to the right of the
     * workspace (in the outer layout). The chat integration module will
     * show/hide this via the “app.toggle-chat” action.                        */
    if (out_chat_box) {
        GtkWidget *chat = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        gtk_widget_set_size_request(chat, 320, -1);        /* reasonable min */
        /* A light label so users know what the panel is about.               */
        GtkWidget *lbl = gtk_label_new("Chat");
        gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
        gtk_widget_set_margin_start(lbl, 8);
        gtk_widget_set_margin_top(lbl, 8);
        gtk_box_append(GTK_BOX(chat), lbl);

        /* Placeholder content; swap with your real chat widget later.        */
        GtkWidget *chat_log = gtk_text_view_new();
        gtk_widget_set_hexpand(chat_log, TRUE);
        gtk_widget_set_vexpand(chat_log, TRUE);
        GtkWidget *chat_scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(chat_scroll), chat_log);
        gtk_box_append(GTK_BOX(chat), chat_scroll);

        *out_chat_box = chat;
    }

    /* Return the fully set up editor/output column to our caller.            */
    return vsplit;
}
