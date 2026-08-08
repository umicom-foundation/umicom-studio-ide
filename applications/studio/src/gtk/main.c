/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gtk/main.c
 *
 * PURPOSE:
 *   Start the GTK4 Umicom Studio IDE frontend inside the Umicom Framework
 *   lifecycle.  Product services remain under applications/studio/src while
 *   reusable capabilities move into Framework through tested vertical slices.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "umicom/studio/bootstrap.h"

static int str_eq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static void log_line(const char *text)
{
    (void)fprintf(stderr, "%s\n", text != NULL ? text : "");
}

static gboolean on_bare_close(GtkWindow *window, gpointer user_data)
{
    GMainLoop *loop = (GMainLoop *)user_data;
    (void)window;
    if (loop != NULL) {
        g_main_loop_quit(loop);
    }
    return FALSE;
}

static int run_bare_gtk(void)
{
    GMainLoop *loop;
    GtkWindow *window;
    GtkWidget *label;

    log_line("[USIDE] bare GTK validation selected");
    gtk_init();
    window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(window, "Umicom Studio IDE - bare GTK validation");
    gtk_window_set_default_size(window, 640, 400);
    label = gtk_label_new("GTK4 is working. Umicom Framework is running.");
    gtk_window_set_child(window, label);
    loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect(window, "close-request", G_CALLBACK(on_bare_close), loop);
    gtk_window_present(window);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    return 0;
}

static void on_test_activate(GtkApplication *application, gpointer user_data)
{
    GtkWindow *window;
    GtkWidget *label;
    (void)user_data;
    window = GTK_WINDOW(gtk_application_window_new(application));
    gtk_window_set_title(window, "Umicom Studio IDE - test window");
    gtk_window_set_default_size(window, 640, 400);
    label = gtk_label_new("GtkApplication and Umicom Framework are working.");
    gtk_window_set_child(window, label);
    gtk_window_present(window);
}

static int run_test_window_app(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "org.umicom.studio.test-window",
        G_APPLICATION_NON_UNIQUE
    );
    int result;
    g_signal_connect(application, "activate", G_CALLBACK(on_test_activate), NULL);
    result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}

static int filter_dev_flags(int argc, char **argv, char ***out_argv)
{
    char **filtered;
    int count = 0;
    int index;
    if (out_argv == NULL) {
        return 0;
    }
    filtered = (char **)malloc((size_t)(argc + 1) * sizeof(*filtered));
    if (filtered == NULL) {
        *out_argv = NULL;
        return 0;
    }
    filtered[count++] = argv[0];
    for (index = 1; index < argc; ++index) {
        if (str_eq(argv[index], "--console") || str_eq(argv[index], "--dev")) {
            continue;
        }
        filtered[count++] = argv[index];
    }
    filtered[count] = NULL;
    *out_argv = filtered;
    return count;
}

static int run_studio(int argc, char **argv)
{
    char **filtered_argv = NULL;
    int filtered_argc;
    int index;
    GtkApplication *application;
    int result;

    for (index = 0; index < argc; ++index) {
        if (str_eq(argv[index], "--bare-gtk")) {
            return run_bare_gtk();
        }
        if (str_eq(argv[index], "--test-window")) {
            return run_test_window_app(argc, argv);
        }
    }

    filtered_argc = filter_dev_flags(argc, argv, &filtered_argv);
    if (filtered_argv == NULL) {
        return 1;
    }

    application = umi_app_new();
    if (application == NULL) {
        free(filtered_argv);
        return 1;
    }
    result = g_application_run(
        G_APPLICATION(application),
        filtered_argc,
        filtered_argv
    );
    g_object_unref(application);
    free(filtered_argv);
    return result;
}

int main(int argc, char **argv)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStatus status;
    int result;

    status = umi_studio_bootstrap_create(&bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "[USIDE] Framework create failed: %s\n",
                      umi_status_text(status));
        return 1;
    }

    status = umi_studio_bootstrap_start(bootstrap);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "[USIDE] Framework start failed: %s\n",
                      umi_status_text(status));
        umi_studio_bootstrap_destroy(bootstrap);
        return 1;
    }

    result = run_studio(argc, argv);
    umi_studio_bootstrap_destroy(bootstrap);
    return result;
}

#ifdef _WIN32
#include <windows.h>

static int wants_console(int argc, char **argv)
{
    const char *environment = getenv("USIDE_DEV");
    int index;
    if (environment != NULL &&
        (str_eq(environment, "1") || _stricmp(environment, "true") == 0)) {
        return 1;
    }
    for (index = 0; index < argc; ++index) {
        if (str_eq(argv[index], "--console") ||
            str_eq(argv[index], "--dev") ||
            str_eq(argv[index], "--test-window") ||
            str_eq(argv[index], "--bare-gtk")) {
            return 1;
        }
    }
    return 0;
}

static void attach_or_allocate_console(void)
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        (void)AllocConsole();
    }
    (void)freopen("CONOUT$", "w", stdout);
    (void)freopen("CONOUT$", "w", stderr);
    (void)freopen("CONIN$", "r", stdin);
    (void)setvbuf(stdout, NULL, _IONBF, 0);
    (void)setvbuf(stderr, NULL, _IONBF, 0);
    (void)SetConsoleOutputCP(CP_UTF8);
    (void)SetConsoleCP(CP_UTF8);
}

int WINAPI WinMain(HINSTANCE instance,
                   HINSTANCE previous_instance,
                   LPSTR command_line,
                   int show_command)
{
    (void)instance;
    (void)previous_instance;
    (void)command_line;
    (void)show_command;
    if (wants_console(__argc, __argv)) {
        attach_or_allocate_console();
    }
    return main(__argc, __argv);
}
#endif
