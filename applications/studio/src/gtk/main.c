/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/main.c   (Single entry with console, test window, and bare GTK)
 *
 * PURPOSE:
 *   Start the application and guarantee we can visualize a window in dev:
 *     - Normal path:   umi_app_new() -> g_application_run()
 *     - --test-window: tiny GtkApplication that always shows a window (NON_UNIQUE)
 *     - --bare-gtk:    NO GtkApplication; create/present a GtkWindow + GMainLoop
 *
 * DEV FLAGS:
 *   --console  / USIDE_DEV=1  -> attach/allocate a console for logs on Win32
 *   --test-window             -> show a tiny window via GtkApplication (non-unique)
 *   --bare-gtk                -> show a tiny window with pure GTK (no GApplication)
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "app.h"  /* umi_app_new(), UmiApp accessors */

static int  str_eq(const char *a, const char *b) { return a && b && strcmp(a,b)==0; }
static void log_line(const char *s) { fprintf(stderr, "%s\n", s); }

/* ------------------------------ bare GTK path ------------------------------ */

static gboolean on_bare_close(GtkWindow *win, gpointer user_data)
{
    (void)win;
    GMainLoop *loop = (GMainLoop*)user_data;
    if (loop) g_main_loop_quit(loop);
    return FALSE; /* propagate default handling */
}

static int run_bare_gtk(void)
{
    log_line("[USIDE] run_bare_gtk(): gtk_init()");
    gtk_init();

    GtkWindow *win = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(win, "USIDE bare GTK window");
    gtk_window_set_default_size(win, 640, 400);

    GtkWidget *label = gtk_label_new("Bare GTK path — if you see this, rendering works.");
    gtk_window_set_child(win, label);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect(win, "close-request", G_CALLBACK(on_bare_close), loop);

    log_line("[USIDE] run_bare_gtk(): presenting window...");
    gtk_window_present(win);

    log_line("[USIDE] run_bare_gtk(): entering main loop");
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    log_line("[USIDE] run_bare_gtk(): exiting");
    return 0;
}

/* ---------------------------- test window path ----------------------------- */

static void on_test_activate(GtkApplication *a, gpointer user_data)
{
    (void)user_data;
    log_line("[USIDE] on_test_activate(): building test window");
    GtkWindow *win = GTK_WINDOW(gtk_application_window_new(a));
    gtk_window_set_title(win, "USIDE test window");
    gtk_window_set_default_size(win, 640, 400);

    GtkWidget *label = gtk_label_new("GtkApplication test — if you see this, app loop is OK.");
    gtk_window_set_child(win, label);

    gtk_window_present(win);
    log_line("[USIDE] on_test_activate(): presented");
}

static int run_test_window_app(int argc, char **argv)
{
    log_line("[USIDE] run_test_window_app(): creating NON_UNIQUE GtkApplication");
    GtkApplication *app = gtk_application_new("org.umicom.studio.testwin", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(on_test_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    fprintf(stderr, "[USIDE] run_test_window_app(): exited with status %d\n", status);
    return status;
}

/* ---------------------------- argv filtering ------------------------------- */
/* Remove dev-only flags so GApplication doesn't error on unknown options. */
static int filter_dev_flags(int argc, char **argv, char ***out_argv)
{
    /* we preserve argv[0]; strip only --console and --dev */
    char **res = (char**)malloc((size_t)(argc + 1) * sizeof(char*));
    int n = 0;
    res[n++] = argv[0];
    for (int i = 1; i < argc; ++i) {
        if (str_eq(argv[i], "--console") || str_eq(argv[i], "--dev"))
            continue; /* drop */
        res[n++] = argv[i];
    }
    res[n] = NULL;
    *out_argv = res;
    return n;
}

/* --------------------------------- normal ---------------------------------- */

int main(int argc, char **argv)
{
    fprintf(stderr, "[USIDE] main(): argc=%d\n", argc);
    for (int i = 0; i < argc; ++i) fprintf(stderr, "  argv[%d] = %s\n", i, argv[i] ? argv[i] : "(null)");

    for (int i = 0; i < argc; ++i) {
        if (str_eq(argv[i], "--bare-gtk")) {
            log_line("[USIDE] main(): --bare-gtk selected");
            return run_bare_gtk();
        }
        if (str_eq(argv[i], "--test-window")) {
            log_line("[USIDE] main(): --test-window selected");
            return run_test_window_app(argc, argv);
        }
    }

    /* Filter out dev-only flags BEFORE giving args to GApplication. */
    char **argv_filtered = NULL;
    int argc_filtered = filter_dev_flags(argc, argv, &argv_filtered);

    log_line("[USIDE] main(): launching umi_app_new() / g_application_run()");
    GtkApplication *app = umi_app_new();  /* should create+present in 'activate' */
    int status = g_application_run(G_APPLICATION(app), argc_filtered, argv_filtered);
    g_object_unref(app);
    free(argv_filtered);
    fprintf(stderr, "[USIDE] main(): g_application_run() returned %d\n", status);
    return status;
}

/*---------------------------- Windows console support -----------------------*/
#ifdef _WIN32
#  include <windows.h>
#  include <stdio.h>

static int wants_console(int argc, char **argv)
{
    const char *env = getenv("USIDE_DEV");
    if (env && (str_eq(env,"1") || _stricmp(env,"true")==0)) return 1;
    for (int i = 0; i < argc; ++i) {
        if (str_eq(argv[i],"--console") || str_eq(argv[i],"--dev")
            || str_eq(argv[i],"--test-window") || str_eq(argv[i],"--bare-gtk"))
            return 1; /* force console for dev/test paths */
    }
    return 0;
}

static void attach_or_alloc_console(void)
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) AllocConsole();
    (void)freopen("CONOUT$", "w", stdout);
    (void)freopen("CONOUT$", "w", stderr);
    (void)freopen("CONIN$",  "r", stdin);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    fprintf(stderr, "[USIDE] console attached\n");
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;

    if (wants_console(__argc, __argv)) {
        attach_or_alloc_console();
        if (!getenv("G_MESSAGES_DEBUG")) _putenv("G_MESSAGES_DEBUG=");
    }
    fprintf(stderr, "[USIDE] WinMain forwarding to main()\n");
    return main(__argc, __argv);
}
#endif
/*---------------------------------------------------------------------------*/
