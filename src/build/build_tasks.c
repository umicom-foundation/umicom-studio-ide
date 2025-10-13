/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_tasks.c
 *
 * PURPOSE:
 *   Provide one-stop high-level calls (build/run/test) for a given project
 *   root by composing:
 *     - UmiBuildSys  : figures out the correct command(s) and argv
 *     - UmiBuildRunner: executes the process and streams output
 *
 * COUPLING:
 *   - Takes an UmiOutputSink* from the caller; does not include UI headers.
 *
 * WARNINGS ELIMINATED:
 *   - Avoids function-pointer typedef collisions by including only the
 *     centralized UmiOutputSink struct (src/include/umi_output_sink.h).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>

#include "src/build/include/build_tasks.h"
#include "src/build/include/build_runner.h"
#include "src/build_system.h"  /* build system detection + argv expansion */

struct _UmiBuildTasks
{
    char            *root;    /* project root directory                       */
    UmiOutputSink   *sink;    /* output destination (owned by caller)         */
    UmiBuildSys     *sys;     /* detected build system                         */
    UmiBuildRunner  *runner;  /* process runner                                 */
};

/* Internal: helper to run arbitrary argv through the runner. */
static gboolean run_argv(UmiBuildTasks *bt, GPtrArray *argv, GError **error)
{
    if (!bt || !bt->runner || !argv) return FALSE;

    /* Ensure NULL-terminated char** for GLib. */
    g_ptr_array_add(argv, NULL);
    char **argvv = (char **)argv->pdata;

    gboolean ok = umi_build_runner_run(bt->runner,
                                       argvv,
                                       NULL,            /* inherit env */
                                       bt->root,        /* cwd         */
                                       NULL,            /* no on_exit  */
                                       NULL);
    if (!ok && error) *error = g_error_new_literal(g_quark_from_static_string("UmiBuildTasks"),
                                                   1, "Failed to launch process");
    /* Caller still owns argv; free it. */
    g_ptr_array_free(argv, TRUE);
    return ok;
}

UmiBuildTasks *umi_build_tasks_new(const char *root, UmiOutputSink *sink)
{
    UmiBuildTasks *bt = g_new0(UmiBuildTasks, 1);
    bt->root   = g_strdup(root ? root : ".");
    bt->sink   = sink;
    bt->sys    = umi_buildsys_detect(bt->root);
    bt->runner = umi_build_runner_new();
    umi_build_runner_set_sink(bt->runner, bt->sink);
    return bt;
}

void umi_build_tasks_free(UmiBuildTasks *bt)
{
    if (!bt) return;
    if (bt->runner) { umi_build_runner_free(bt->runner); bt->runner = NULL; }
    if (bt->sys)    { umi_buildsys_free(bt->sys);        bt->sys    = NULL; }
    g_clear_pointer(&bt->root, g_free);
    g_free(bt);
}

void umi_build_tasks_set_sink(UmiBuildTasks *bt, UmiOutputSink *sink)
{
    if (!bt) return;
    bt->sink = sink;
    umi_build_runner_set_sink(bt->runner, bt->sink);
}

gboolean umi_build_tasks_build(UmiBuildTasks *bt, GError **error)
{
    if (!bt || !bt->sys) return FALSE;
    GPtrArray *argv = umi_buildsys_build_argv(bt->sys);
    return run_argv(bt, argv, error);
}

gboolean umi_build_tasks_run(UmiBuildTasks *bt, GError **error)
{
    if (!bt || !bt->sys) return FALSE;
    GPtrArray *argv = umi_buildsys_run_argv(bt->sys);
    return run_argv(bt, argv, error);
}

gboolean umi_build_tasks_test(UmiBuildTasks *bt, GError **error)
{
    if (!bt || !bt->sys) return FALSE;
    GPtrArray *argv = umi_buildsys_test_argv(bt->sys);
    return run_argv(bt, argv, error);
}

const char *umi_build_tasks_root(const UmiBuildTasks *bt)
{
    return bt ? bt->root : NULL;
}
