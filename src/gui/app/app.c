/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app.c
 *
 * PURPOSE:
 *   Implement the top-level GTK application shell with **minimal coupling**.
 *   We create a single main window that looks like a tiny code editor:
 *     - top toolbar (Run / Stop / Save)
 *     - left  : "File Tree" placeholder (you can swap in a real widget later)
 *     - center: editor **tabs** (GtkNotebook of GtkTextView / GtkSourceView)
 *     - bottom: Output / Problems tabs
 *
 *   All optional features are referenced via **weak symbols** so this file
 *   compiles and links cleanly even if build, editor or pipeline modules are
 *   not part of the current build. That keeps iteration fast and simple.
 *
 * DESIGN NOTES:
 *   - We avoid changing public headers. Extra pointers (notebook, labels…)
 *     are attached to the window object with g_object_set_data() so we do
 *     not need to grow `struct UmiApp` in app.h.
 *   - We guard GtkSourceView usage; if the dev machine doesn't have it we
 *     fall back to a plain GtkTextView, so the IDE always runs.
 *   - We fix a previous bug where `on_activate()` built **two** UIs: the
 *     richer one from build_main_ui() was immediately replaced by a simple
 *     placeholder hierarchy. Now we build **once**.
 *
 * QUICK TOUR (how GTK flows here):
 *   main()          -> umi_app_new() returns GtkApplication (NON_UNIQUE in dev)
 *   g_application_run()
 *     ├─ "startup"  -> on_startup()  (lightweight init, hash-map etc.)
 *     └─ "activate" -> on_activate() (create window + build_main_ui())
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>                  /* core GTK API we use everywhere      */
#include <glib.h>                     /* GLib helpers: hash tables, strings  */
#include "splash.h"                   /* splash screen API                   */

/* GtkSourceView is optional – it gives nicer code editing features.          */
/* We include it if present; otherwise we'll fall back to GtkTextView.        */
#if defined(__has_include)
#  if __has_include(<gtksourceview/gtksource.h>)
#    include <gtksourceview/gtksource.h>
#    define USIDE_HAVE_GTK_SOURCEVIEW 1
#  endif
#endif

#include "app.h"                      /* UmiApp handle + public accessors    */

/*------------------------------------------------------------------------------
 * Internal string keys used with g_object_set_data()/get_data() on the window.
 * We keep these in one place to avoid typos.
 *----------------------------------------------------------------------------*/
#define KEY_NB_EDITORS      "uside.nb.editors"      /* GtkNotebook* (center)   */
#define KEY_NB_BOTTOM       "uside.nb.bottom"       /* GtkNotebook* (bottom)   */
#define KEY_OUTPUT_SCROLLER "uside.output.scroller" /* GtkScrolledWindow*      */
#define KEY_STATUS_LABEL    "uside.status.label"    /* GtkLabel* (status bar)  */
#define KEY_FILETREE        "uside.filetree.widget" /* GtkWidget* (left pane)  */

/*------------------------------------------------------------------------------
 * Forward declarations of small helpers so we can keep functions ordered
 * top-to-bottom in a readable way.
 *----------------------------------------------------------------------------*/
static GtkWidget *make_toolbar(struct UmiApp *ua);
static GtkWidget *create_editor_scroller(void);
static GtkWidget *add_editor_tab(struct UmiApp *ua, const char *title);
static GtkWidget *make_output_scroller(void);
static void       append_output_line(GtkWidget *output_scroller, const char *text);
static void       build_main_ui(struct UmiApp *ua);

/*------------------------------------------------------------------------------
 * Optional/weak cross-module hooks.
 *
 * These let us call into other subsystems **if present** without creating
 * a hard link-time dependency. The linker will resolve them when those
 * objects are included in the build, otherwise the pointer is NULL.
 *----------------------------------------------------------------------------*/
#if defined(__GNUC__) || defined(__clang__)
/* Start/Stop the run pipeline (build + execute). */
__attribute__((weak)) gboolean umi_run_pipeline_start(gpointer out, gpointer problems, gpointer reserved);
__attribute__((weak)) void     umi_run_pipeline_stop(void);

/* Save the current editor (implementation can live in editor module). */
typedef struct _UmiEditor UmiEditor;              /* forward decl to avoid pulling headers */
__attribute__((weak)) gboolean umi_editor_save(UmiEditor *ed, GError **error);
#else
/* MSVC (or other) fallback: classic function pointers defaulting to NULL.     */
gboolean (*umi_run_pipeline_start)(gpointer,gpointer,gpointer) = NULL;
void     (*umi_run_pipeline_stop)(void) = NULL;
typedef struct _UmiEditor UmiEditor;
gboolean (*umi_editor_save)(UmiEditor*,GError**) = NULL;
#endif

/*------------------------------------------------------------------------------
 * One UmiApp per GtkApplication – stored in a tiny map. We use direct keys
 * and values since both are stable pointers managed elsewhere.
 *----------------------------------------------------------------------------*/
static GHashTable *g_app_map = NULL;

/*==============================================================================
 * UI building blocks
 *============================================================================*/

/* Create an editor widget inside a scroller.
 * If GtkSourceView is available, we use it; otherwise a plain GtkTextView.
 * Returning the **scroller** makes it easy to tab this into a GtkNotebook.
 */
static GtkWidget *create_editor_scroller(void)
{
    /* Create a text editing widget. GtkSourceView gives syntax features.     */
#if USIDE_HAVE_GTK_SOURCEVIEW
    GtkWidget *text = gtk_source_view_new();            /* nicer code editing */
#else
    GtkWidget *text = gtk_text_view_new();              /* portable fallback  */
#endif
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text), TRUE);   /* code fonts   */
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_NONE); /* no wrap */

    /* Wrap it in a scroller so long files don't inflate the layout.          */
    GtkWidget *scr = gtk_scrolled_window_new();         /* scroller container  */
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), text); /* pack     */
    return scr;                                         /* return the scroller */
}

/* Append a new tab to the editor notebook and focus it.
 * We store the tab child as a scroller; the contained text view can be got
 * back with gtk_scrolled_window_get_child().
 */
static GtkWidget *add_editor_tab(struct UmiApp *ua, const char *title)
{
    GtkWidget   *scr = create_editor_scroller();        /* editor in scroller */
    GtkWidget   *lbl = gtk_label_new(title ? title : "untitled"); /* tab text  */

    /* Fetch the notebook from window data (no header changes needed).        */
    GtkNotebook *nb  = GTK_NOTEBOOK(g_object_get_data(G_OBJECT(ua->win), KEY_NB_EDITORS));
    int          idx = gtk_notebook_append_page(nb, scr, lbl);   /* add page   */
    gtk_notebook_set_current_page(nb, idx);                       /* focus it   */

    /* Return the underlying text view in case callers want to manipulate it. */
    return gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scr));
}

/* Create a read-only output pane (TextView in a scroller). */
static GtkWidget *make_output_scroller(void)
{
    GtkWidget *tv  = gtk_text_view_new();               /* plain text sink     */
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(tv), TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), FALSE);

    GtkWidget *scr = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv);
    return scr;
}

/* Convenience: append a line of text to our output pane. */
static void append_output_line(GtkWidget *output_scroller, const char *text)
{
    if (!output_scroller || !text) return;
    GtkTextView   *tv  = GTK_TEXT_VIEW(gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(output_scroller)));
    GtkTextBuffer *buf = gtk_text_view_get_buffer(tv);  /* get backing buffer  */
    GtkTextIter    end;
    gtk_text_buffer_get_end_iter(buf, &end);            /* move to end         */
    gtk_text_buffer_insert(buf, &end, text, -1);        /* append UTF-8 text   */
    gtk_text_buffer_insert(buf, &end, "\n", 1);         /* newline for clarity */
}

/*------------------------------------------------------------------------------
 * Toolbar callbacks. These are tiny adaptors that call weak hooks when those
 * modules are linked in, and safely no-op when they are not.
 *----------------------------------------------------------------------------*/
static void on_run_clicked(gpointer user)
{
    (void)user;                                         /* we don't use it     */
    if (umi_run_pipeline_start) {                       /* present? call it    */
        /* You can wire real output/problem sinks here (we pass NULLs now).   */
        (void)umi_run_pipeline_start(NULL, NULL, NULL);
    }
}

static void on_stop_clicked(gpointer user)
{
    (void)user;
    if (umi_run_pipeline_stop) umi_run_pipeline_stop(); /* stop if available   */
}

static void on_save_clicked(gpointer user)
{
    /* If an editor backend provides umi_editor_save(), call it.               */
    struct UmiApp *ua = (struct UmiApp*)user;
    if (umi_editor_save && ua) {
        /* We don't track a strong editor object here; that's fine – many      */
        /* backends simply save the active tab/buffer internally.              */
        GError *err = NULL;
        if (!umi_editor_save(NULL, &err) && err) {      /* best-effort call    */
            g_warning("Save failed: %s", err->message);
            g_error_free(err);
        }
    }
}

/* Build a simple HBox toolbar with three buttons. */
/* Keeping it code-only avoids any .ui builder files or CSS dependencies. */
static GtkWidget *make_toolbar(struct UmiApp *ua)
{
    GtkWidget *box  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);/* row with gap */

    GtkWidget *run  = gtk_button_new_with_label("Run");          /* [Run]        */
    GtkWidget *stop = gtk_button_new_with_label("Stop");         /* [Stop]       */
    GtkWidget *save = gtk_button_new_with_label("Save");         /* [Save]       */

    gtk_box_append(GTK_BOX(box), run);
    gtk_box_append(GTK_BOX(box), stop);
    gtk_box_append(GTK_BOX(box), save);

    /* Connect signals; use _swapped so the UmiApp* is passed as the callback
       instance data (nice and compact, no extra closures required).          */
    g_signal_connect_swapped(run,  "clicked", G_CALLBACK(on_run_clicked),  ua);
    g_signal_connect_swapped(stop, "clicked", G_CALLBACK(on_stop_clicked), ua);
    g_signal_connect_swapped(save, "clicked", G_CALLBACK(on_save_clicked), ua);

    /* Add tiny margins so the bar doesn't hug the window edges too tightly.  */
    gtk_widget_set_margin_start (box, 6);
    gtk_widget_set_margin_end   (box, 6);
    gtk_widget_set_margin_top   (box, 6);
    gtk_widget_set_margin_bottom(box, 6);

    return box;                                           /* give the bar back */
}

/*------------------------------------------------------------------------------
 * Build the **entire** main window content. This is the "Code editor" layout.
 * IMPORTANT: This function is now the single place that builds the UI; we no
 * longer build a second placeholder layout in on_activate().
 *----------------------------------------------------------------------------*/
static void build_main_ui(struct UmiApp *ua)
{
    /* Root container: a vertical box hosting (top→bottom) toolbar, split,     */
    /* bottom notebook (Output/Problems) and a tiny status row.                */
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    ua->root = root;                                     /* keep a handle      */

    /* --- Toolbar row ------------------------------------------------------ */
    gtk_box_append(GTK_BOX(root), make_toolbar(ua));     /* add toolbar first  */

    /* --- Middle split: left ("File Tree") and the editor/bottom stack ------ */
    GtkWidget *hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(hpaned, TRUE);                /* fill window width  */
    gtk_widget_set_vexpand(hpaned, TRUE);                /* and height         */
    gtk_box_append(GTK_BOX(root), hpaned);               /* pack into root     */

    /* Left child: placeholder you can later swap for a real tree view.        */
    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *left_label = gtk_label_new("File Tree");  /* visible title      */
    gtk_box_append(GTK_BOX(left), left_label);
    gtk_paned_set_start_child(GTK_PANED(hpaned), left);  /* put on the left    */
    gtk_widget_set_size_request(left, 240, -1);          /* typical IDE width  */
    g_object_set_data(G_OBJECT(ua->win), KEY_FILETREE, left);

    /* Right side is itself a vertical split: editors (top) over bottom panels */
    GtkWidget *vpaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_set_end_child(GTK_PANED(hpaned), vpaned);  /* put vsplit on right*/

    /* Center-top: an editor notebook (tabs).                                  */
    GtkNotebook *nb_editors = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_scrollable(nb_editors, TRUE);       /* many tabs scroll   */
    gtk_paned_set_start_child(GTK_PANED(vpaned), GTK_WIDGET(nb_editors));
    g_object_set_data(G_OBJECT(ua->win), KEY_NB_EDITORS, nb_editors);

    /* Center-bottom: a notebook with Output and Problems pages.               */
    GtkNotebook *nb_bottom = GTK_NOTEBOOK(gtk_notebook_new());
    g_object_set_data(G_OBJECT(ua->win), KEY_NB_BOTTOM, nb_bottom);

    /* First bottom page: Output console (read-only TextView in scroller).     */
    GtkWidget *output_scr = make_output_scroller();
    gtk_notebook_append_page(nb_bottom, output_scr, gtk_label_new("Output"));
    g_object_set_data(G_OBJECT(ua->win), KEY_OUTPUT_SCROLLER, output_scr);

    /* Second bottom page: Problems placeholder (wire your real problems pane). */
    GtkWidget *problems_placeholder = gtk_label_new("Problems pane (TBD)");
    gtk_notebook_append_page(nb_bottom, problems_placeholder, gtk_label_new("Problems"));

    /* Pack the bottom notebook into the lower half of the vertical split.     */
    gtk_paned_set_end_child(GTK_PANED(vpaned), GTK_WIDGET(nb_bottom));

    /* Give the vertical split a reasonable initial ratio.                     */
    gtk_paned_set_position(GTK_PANED(vpaned), 520);      /* pixels from top    */

    /* --- Status row ------------------------------------------------------- */
    GtkWidget *status = gtk_label_new("Ready");          /* simple status text */
    gtk_widget_set_margin_start (status, 6);
    gtk_widget_set_margin_end   (status, 6);
    gtk_widget_set_margin_top   (status, 4);
    gtk_widget_set_margin_bottom(status, 6);
    gtk_box_append(GTK_BOX(root), status);
    g_object_set_data(G_OBJECT(ua->win), KEY_STATUS_LABEL, status);

    /* Finally, make this whole hierarchy the window's child and show it.      */
    gtk_window_set_child(ua->win, root);

    /* Quality-of-life: open one blank editor tab so it feels alive on start.  */
    (void)add_editor_tab(ua, "untitled");

    /* Optional: log a line to the Output pane so we see it wired up.          */
    append_output_line(output_scr, "[USIDE] Output pane is ready.");
}

/*==============================================================================
 * Application lifecycle
 *============================================================================*/

/* "startup" is emitted once; we just ensure the map exists.
 * IMPORTANT: Do NOT create GTK widgets here! GTK isn't fully initialized yet.
 * Widgets must be created in on_activate() or later.
 */
static void on_startup(GtkApplication *app, gpointer user_data)
{
    (void)app; (void)user_data;
    g_message("[USIDE] app.c: on_startup()");
    
    /* Initialize our app-to-handle mapping table. */
    if (!g_app_map) {
        g_app_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    }
    
    /* NOTE: We do NOT create the splash here anymore. Creating GTK widgets
     * during on_startup() can cause crashes or undefined behavior because
     * GTK's initialization isn't complete yet. The splash will be created
     * in on_activate() instead, which is the proper place for UI creation.
     */
}

/* "activate" is emitted when the app should present its primary window.
 * This is the correct place to create all GTK widgets, including the splash.
 */
static void on_activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;
    g_message("[USIDE] app.c: on_activate()");

    /* STEP 1: Show splash FIRST (before heavy window building).
     * We create the splash as the very first thing so users see immediate
     * feedback that the application is loading.
     */
    UmiSplash *splash = umi_splash_new(
        "Umicom Studio IDE",
        "Starting up…",
        0  /* no auto-close; we'll close it manually after main window shows */
    );
    umi_splash_show(splash, NULL);  /* NULL = no parent window yet */
    
    /* Store splash handle on the app so we can close it later. The
     * GDestroyNotify will automatically free the splash when the app exits.
     */
    g_object_set_data_full(G_OBJECT(app), "umicom-splash", splash,
                          (GDestroyNotify)umi_splash_free);

    /* STEP 2: Retrieve (or create) our lightweight handle for this GtkApplication. */
    struct UmiApp *ua = g_app_map ? g_hash_table_lookup(g_app_map, app) : NULL;
    if (!ua) {
        ua = g_new0(struct UmiApp, 1);
        ua->app = app;
        if (!g_app_map) g_app_map = g_hash_table_new(g_direct_hash, g_direct_equal);
        g_hash_table_insert(g_app_map, app, ua);
    }

    /* STEP 3: Create the main toplevel and set a sensible default size/title. */
    ua->win = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(ua->win, "Umicom Studio IDE");
    gtk_window_set_default_size(ua->win, 1280, 800);

    /* STEP 4: Build the COMPLETE UI exactly once. */
    build_main_ui(ua);

    /* STEP 5: Present the window on screen. */
    g_message("[USIDE] app.c: presenting main window…");
    gtk_window_present(ua->win);
    g_message("[USIDE] app.c: …presented");
    
    /* STEP 6: Close splash after a brief grace period (300ms).
     * This gives the main window time to fully render before the splash
     * disappears, creating a smooth transition for users.
     */
    g_timeout_add(300, (GSourceFunc)umi_splash_close, splash);
}

/*==============================================================================
 * Public API (kept tiny and stable)
 *============================================================================*/

/* Create a GtkApplication instance and wire our signals.                      
 * We use NON_UNIQUE during development to avoid the "silent forward & exit"
 * behaviour when another instance is already running. It guarantees you see
 * a window while iterating. You can switch to UNIQUE later if desired.
 */
GtkApplication *umi_app_new(void)
{
    GtkApplication *app =
        gtk_application_new("org.umicom.studio",
                            G_APPLICATION_NON_UNIQUE |   /* dev-friendly      */
                            G_APPLICATION_HANDLES_OPEN); /* future-proof      */

    g_signal_connect(app, "startup",  G_CALLBACK(on_startup),  NULL);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    g_message("[USIDE] app.c: umi_app_new() created GtkApplication");
    return app;
}

/* Map GtkApplication → UmiApp handle (NULL if not found). */
struct UmiApp *umi_app_handle(GtkApplication *app)
{
    if (!g_app_map) return NULL;
    return g_hash_table_lookup(g_app_map, app);
}

/* Convenience accessors; these are tiny wrappers and NULL-safe. */
GtkWindow *umi_app_window(struct UmiApp *ua) { return ua ? ua->win : NULL; }
UmiEditor *umi_app_editor(struct UmiApp *ua) { (void)ua; return NULL; } /* placeholder */

/*---------------------------------------------------------------------------*/
/*  END OF FILE */