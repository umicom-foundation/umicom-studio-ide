/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/app.c
 *
 * PURPOSE:
 *   Application shell: create/manage the GtkApplication and main window.
 *   This module handles:
 *     - Creating and initializing the GtkApplication
 *     - Managing the main window lifecycle
 *     - Providing accessor functions for app state (UmiApp handle)
 *     - Wiring up application-level actions and keybindings
 *
 * DESIGN:
 *   - Pure C; no XML resources or CSS files
 *   - Loosely coupled: minimal cross-module dependencies
 *   - Stability-focused: safe pointer handling throughout
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>        /* GTK4 public API */
#include <glib.h>           /* GLib utilities */
#include "app.h"            /* Our public API (UmiApp, accessors) */
#include "app_actions.h"    /* Keymap wiring */
#include "splash.h"         /* Splash screen helpers */
#include "theme.h"          /* Theme/styling stubs */

/* Forward declare the window builder from window.c */
extern GtkWidget *window_new(GtkApplication *app);

/* --- Global app instance (lifetime: entire application) --- */
/* This is safe because we only ever create one GtkApplication per process. */
static UmiApp g_app_instance = {0};

/*-----------------------------------------------------------------------------
 * on_activate
 *
 * PURPOSE:
 *   GtkApplication "activate" signal handler. Called when the app should
 *   create its main window and present it to the user. This is where all
 *   UI initialization happens.
 *
 * PARAMETERS:
 *   app         : GtkApplication* - the application instance
 *   user_data   : gpointer - unused (we use the global g_app_instance instead)
 *
 * SIDE EFFECTS:
 *   - Creates the main window via window_new()
 *   - Stores window pointer in g_app_instance
 *   - Presents the window to the user
 *---------------------------------------------------------------------------*/
static void on_activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;  /* Suppress unused parameter warning */

    /* Build the full UI from scratch using pure C (no XML resources). */
    GtkWidget *win = window_new(app);
    if (!win) {
        g_warning("[app.c] on_activate: Failed to create main window");
        return;
    }

    /* Store the window pointer in our global app instance for later access. */
    g_app_instance.win = GTK_WINDOW(win);

    /* Present the window to the user (show it, make it visible). */
    gtk_window_present(GTK_WINDOW(win));

    g_message("[app.c] on_activate: main window created and presented");
}

/*-----------------------------------------------------------------------------
 * umi_app_new
 *
 * PURPOSE:
 *   Create and initialize a new GtkApplication for the Umicom Studio IDE.
 *   This is the main entry point for app initialization. It must be called
 *   exactly once from main(), before g_application_run().
 *
 * RETURNS:
 *   GtkApplication* - a newly created application instance (caller owns it).
 *                     Returns NULL on failure (rare; indicates system issues).
 *
 * SIDE EFFECTS:
 *   - Initializes g_app_instance global
 *   - Registers the "activate" signal handler
 *   - Wires up application actions (see app_actions.c)
 *---------------------------------------------------------------------------*/
GtkApplication *umi_app_new(void)
{
    /* Create a new GtkApplication instance.
     * app_id:  "org.umicom.studio-ide" identifies us uniquely to the OS.
     * flags:   G_APPLICATION_DEFAULT_FLAGS allows multiple invocations.
     */
    GtkApplication *app = gtk_application_new(
        "org.umicom.studio-ide",
        G_APPLICATION_DEFAULT_FLAGS
    );

    if (!app) {
        g_error("[app.c] umi_app_new: Failed to create GtkApplication");
        return NULL;
    }

    /* Cache the app pointer in our global instance struct. */
    g_app_instance.app = app;
    g_app_instance.editor = NULL;  /* Editor not yet created */

    /* Wire the "activate" signal so we create the UI when the app starts. */
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    /* Fill in the keymap/action callbacks (from app_actions.c).
     * This wires up menus, keyboard shortcuts, toolbar buttons, etc. */
    UmiKeymapCallbacks keymap = {0};
    umi_app_fill_keymap(app, &keymap);

    g_message("[app.c] umi_app_new: application initialized");
    return app;
}

/*-----------------------------------------------------------------------------
 * umi_app_handle
 *
 * PURPOSE:
 *   Look up the UmiApp controller from a GtkApplication pointer.
 *   This is a convenience accessor for code that has the GtkApplication
 *   but needs to access the broader app state (window, editor, etc.).
 *
 * PARAMETERS:
 *   app : GtkApplication* - the app instance to look up
 *
 * RETURNS:
 *   UmiApp* - pointer to the global app instance if found; NULL otherwise.
 *
 * NOTE:
 *   In this simple design we only ever have one app instance, so this is
 *   essentially an identity function. In larger systems you might maintain
 *   a hash table of multiple apps indexed by their GtkApplication pointer.
 *---------------------------------------------------------------------------*/
UmiApp *umi_app_handle(GtkApplication *app)
{
    /* Only match if this is our global app instance. */
    if (app == g_app_instance.app) {
        return &g_app_instance;
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * umi_app_window
 *
 * PURPOSE:
 *   Fetch the main GtkWindow* from the app controller.
 *   May return NULL if the window has not been created yet (before "activate"
 *   signal) or if the window has been closed.
 *
 * PARAMETERS:
 *   ua : UmiApp* - the app instance (from umi_app_handle)
 *
 * RETURNS:
 *   GtkWindow* - pointer to the main window, or NULL if unavailable
 *---------------------------------------------------------------------------*/
GtkWindow *umi_app_window(UmiApp *ua)
{
    if (!ua) return NULL;
    return ua->win;
}

/*-----------------------------------------------------------------------------
 * umi_app_editor
 *
 * PURPOSE:
 *   Fetch the editor handle from the app controller.
 *   The editor is created lazily on demand and stored here for global access.
 *   This allows any part of the app (actions, menus, dialogs) to reference
 *   the editor without needing to pass it through many function parameters.
 *
 * PARAMETERS:
 *   ua : UmiApp* - the app instance (from umi_app_handle)
 *
 * RETURNS:
 *   struct _UmiEditor* - pointer to the editor, or NULL if not yet created
 *
 * NOTE:
 *   The struct _UmiEditor is forward-declared here to keep this module
 *   loosely coupled from the editor implementation. If you need the full
 *   definition, include "editor.h" (or whichever file defines it).
 *---------------------------------------------------------------------------*/
struct _UmiEditor *umi_app_editor(UmiApp *ua)
{
    if (!ua) return NULL;
    return ua->editor;
}

/*---------------------------------------------------------------------------*/
/*  END OF FILE */
/*---------------------------------------------------------------------------*/