/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_runner.c
 *
 * PURPOSE:
 *   Minimal GLib/GIO-based process runner that streams stdout/stderr lines
 *   to an UmiOutputSink. Used by build systems and search tools.
 *
 * SECURITY:
 *   - No shell invocation; argv is passed as an array (no injection).
*   Concrete implementation of UmiBuildRunner. Spawns an external process
 *   (e.g., ninja/make/ctest), reads stdout/stderr line-by-line, and forwards
 *   them to an injected UmiOutputSink. Reports final exit status via callback.
 *
 * KEY POINTS:
 *   - This module *never* includes UI or pane headers. It depends only on
 *     umi_output_sink.h which is a small decoupled interface.
 *   - Uses non-blocking async reads (GDataInputStream + read_line_async)
 *     to integrate naturally with the GLib main loop.
 *   - Environment handling: GLib expects a writable char** for custom env,

 *   - Environment/cwd are applied explicitly via GSubprocessLauncher.
*     so we duplicate the provided envp (if any) and free our copy.
 *
 * WARNINGS ELIMINATED:
 *   - No nested comment sequences in docstrings (avoid "/*" patterns).
 *   - No 'discarded-qualifiers' warnings when passing env via launcher:
 *     we call g_strdupv() to produce a writable environment vector.

 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include "build_runner.h"

/*-----------------------------------------------------------------------------
 * Private structure — not exposed in the header (keeps ABI/API clean).
 *---------------------------------------------------------------------------*/

struct UmiBuildRunner {
    UmiOutputSink *sink; /* not owned */
};

UmiBuildRunner *umi_build_runner_new(void) {
    return g_new0(UmiBuildRunner, 1);
}

void umi_build_runner_free(UmiBuildRunner *br) {
    g_free(br);
}

static void stream_lines(GInputStream *stream, gboolean is_err, UmiOutputSink *sink) {
    if (!stream || !sink) return;
    GDataInputStream *din = g_data_input_stream_new(stream);
    gsize len = 0;
    GError *err = NULL;
    for (;;) {
        gchar *line = g_data_input_stream_read_line(din, &len, NULL, &err);
        if (err) { g_clear_error(&err); break; }
        if (!line) break;
        umi_output_sink_append_line(sink, line);
        if (is_err) umi_output_sink_append_err_line(sink, line);
        g_free(line);
    }
    g_object_unref(din);
}

typedef struct ThreadCtx {
    GInputStream *stream;
    gboolean      is_err;
    UmiOutputSink *sink;
} ThreadCtx;

static gpointer reader_thread(gpointer data) {
    ThreadCtx *ctx = (ThreadCtx*)data;
    stream_lines(ctx->stream, ctx->is_err, ctx->sink);
    g_object_unref(ctx->stream);
    g_free(ctx);
    return NULL;
}

void umi_build_runner_set_sink(UmiBuildRunner *br, UmiOutputSink *sink) {
    if (!br) return;
    br->sink = sink;
}

static gchar **build_argv_vector(const char *exe, char *const argv[]) {
    GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
    g_ptr_array_add(a, g_strdup(exe));
    if (argv) {
        for (int i = 0; argv[i]; ++i) {
            g_ptr_array_add(a, g_strdup(argv[i]));
        }
    }
    g_ptr_array_add(a, NULL);
    return (gchar**)g_ptr_array_free(a, FALSE);
}

static gchar **build_env_vector(char *const envp[]) {
    if (!envp) return NULL;
    GPtrArray *a = g_ptr_array_new_with_free_func(g_free);
    for (int i = 0; envp[i]; ++i)
        g_ptr_array_add(a, g_strdup(envp[i]));
    g_ptr_array_add(a, NULL);
    return (gchar**)g_ptr_array_free(a, FALSE);
}

gboolean umi_build_runner_run(UmiBuildRunner *br,
                              const char *cwd,
                              const char *exe,
                              char *const argv[],
                              char *const envp[],
                              gboolean merge_stderr) {
    if (!br || !exe) return FALSE;

    GSubprocessLauncher *launcher = g_subprocess_launcher_new(
        G_SUBPROCESS_FLAGS_STDOUT_PIPE |
        (merge_stderr ? G_SUBPROCESS_FLAGS_STDERR_MERGE : G_SUBPROCESS_FLAGS_STDERR_PIPE));

    if (cwd && *cwd) g_subprocess_launcher_set_cwd(launcher, cwd);

    gchar **envv = build_env_vector(envp);
    if (envv) {
        for (int i = 0; envv[i]; ++i) {
            const char *eq = strchr(envv[i], '=');
            if (!eq) continue;
            gchar *k = g_strndup(envv[i], eq - envv[i]);
            g_subprocess_launcher_setenv(launcher, k, eq + 1, TRUE);
            g_free(k);
        }
        g_strfreev(envv);
    }

    gchar **argvv = build_argv_vector(exe, argv);
    GError *err = NULL;
    GSubprocess *proc = g_subprocess_launcher_spawnv(launcher, (const char * const *)argvv, &err);
    g_object_unref(launcher);
    g_strfreev(argvv);

    if (!proc) {
        if (br->sink) {
            umi_output_sink_append_err_line(br->sink, err && err->message ? err->message : "spawn failed");
        }
        g_clear_error(&err);
        return FALSE;
    }

    /* Readers for stdout/stderr */
    ThreadCtx *tout = g_new0(ThreadCtx, 1);
    tout->stream = G_INPUT_STREAM(g_object_ref(g_subprocess_get_stdout_pipe(proc)));
    tout->is_err = FALSE;
    tout->sink   = br->sink;

    GThread *th_out = g_thread_new("umi-br-out", reader_thread, tout);
    GThread *th_err = NULL;

    if (!merge_stderr) {
        ThreadCtx *terr = g_new0(ThreadCtx, 1);
        terr->stream = G_INPUT_STREAM(g_object_ref(g_subprocess_get_stderr_pipe(proc)));
        terr->is_err = TRUE;
        terr->sink   = br->sink;
        th_err = g_thread_new("umi-br-err", reader_thread, terr);
    }

    /* Wait for process */
    gboolean ok = g_subprocess_wait(proc, NULL, &err);
    if (err) { g_clear_error(&err); }

    g_thread_join(th_out);
    if (th_err) g_thread_join(th_err);

    gint ec = 0;
    if (ok) ok = g_subprocess_get_if_exited(proc) && (ec = g_subprocess_get_exit_status(proc)) == 0;

    g_object_unref(proc);
    return ok;
}