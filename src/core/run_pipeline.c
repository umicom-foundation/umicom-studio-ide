/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/run_pipeline.c
 * PURPOSE: Launch and manage external run/build commands via the unified
 *          build runner. Pure C (C17), GLib only; no UI/status/XML deps.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                 /* gboolean, GError, GPtrArray, g_message, etc. */
#include <string.h>               /* memcpy (rare), strcmp, etc.                  */

#include "run_pipeline.h"         /* Declares umi_run_pipeline_start/stop         */
#include "run_config.h"           /* Load/convert run config                      */
#include "../../build/include/build_runner.h"  /* Runner ctor + run               */

/*-----------------------------------------------------------------------------
 * Static runner state
 *---------------------------------------------------------------------------*/
static UmiBuildRunner *g_runner = NULL;

/*-----------------------------------------------------------------------------
 * Exit callback: invoked when child process terminates.
 *---------------------------------------------------------------------------*/
static void
on_exit(gpointer user, int code)
{
    (void)user; /* unused */
    g_message("run-pipeline: process exited with code %d", code);
}

/*-----------------------------------------------------------------------------
 * Helper: convert GPtrArray (of char*) to a newly-allocated NULL-terminated
 *         vector suitable for execv-style APIs.
 * OWNERSHIP: caller must g_free() the returned vector. Strings are NOT copied.
 *---------------------------------------------------------------------------*/
static char * const *
argv_from_ptr_array(const GPtrArray *arr)
{
    if (!arr || arr->len == 0) {
        char **empty = g_new0(char *, 1);
        return (char * const *)empty;
    }
    char **vec = g_new0(char *, arr->len + 1);
    for (guint i = 0; i < arr->len; i++) {
        vec[i] = (char *)arr->pdata[i]; /* pass through; no deep copy */
    }
    vec[arr->len] = NULL;
    return (char * const *)vec;
}

/*-----------------------------------------------------------------------------
 * Start pipeline using config: builds argv/envp and launches via runner.
 *
 * Signature per run_pipeline.h:
 *   gboolean umi_run_pipeline_start(UmiOutputPane *out,
 *                                   UmiProblemList *plist,
 *                                   GError **err);
 *
 * NOTE: We don’t currently use 'out' or 'plist' here; wiring diagnostics to
 * panes can be added later once runner exposes pipe capture.
 *---------------------------------------------------------------------------*/
gboolean
umi_run_pipeline_start(UmiOutputPane *out, UmiProblemList *plist, GError **err)
{
    (void)out;   /* not used yet */
    (void)plist; /* not used yet */

    /* Load the run configuration (cwd, argv_line, env_multiline). */
    UmiRunConfig *rc = umi_run_config_load();
    if (G_UNLIKELY(!rc)) {
        if (err) g_set_error(err, g_quark_from_static_string("uside-run"), 1,
                             "failed to load run configuration");
        return FALSE;
    }

    /* Build argv (GPtrArray) and convert to NULL-terminated vector. */
    GPtrArray *argv_arr = umi_run_config_to_argv(rc);
    if (G_UNLIKELY(!argv_arr || argv_arr->len == 0 || argv_arr->pdata[0] == NULL)) {
        if (err) g_set_error(err, g_quark_from_static_string("uside-run"), 2,
                             "run configuration produced empty argv");
        if (argv_arr) g_ptr_array_free(argv_arr, TRUE);
        umi_run_config_free(rc);
        return FALSE;
    }
    char * const *argvv = argv_from_ptr_array(argv_arr);

    /* Build envp (NULL-terminated).  We’ll free with g_strfreev(). */
    gchar **envp_glib = umi_run_config_to_envp(rc);
    char * const *envp = (char * const *)envp_glib; /* cast for runner API */

    /* Determine working directory. */
    const char *cwd = (rc->cwd && *rc->cwd) ? rc->cwd : ".";

    /* Ensure we have a runner. Constructor takes no arguments. */
    if (g_runner == NULL) {
        g_runner = umi_build_runner_new();
        if (G_UNLIKELY(g_runner == NULL)) {
            if (err) g_set_error(err, g_quark_from_static_string("uside-run"), 3,
                                 "failed to create build runner");
            g_free((gpointer)argvv);
            if (envp_glib) g_strfreev(envp_glib);
            g_ptr_array_free(argv_arr, TRUE);
            umi_run_config_free(rc);
            return FALSE;
        }
    }

    /* Launch with correct parameter order: (runner, argv, envp, cwd, on_exit, user, err) */
    GError *gerr = NULL;
    gboolean ok = umi_build_runner_run(
        g_runner,
        argvv,
        envp,           /* NULL to inherit, or env vector */
        cwd,
        on_exit,
        NULL,           /* user data for callback */
        &gerr
    );

    /* Cleanup temporaries (runner is kept for reuse). */
    g_free((gpointer)argvv);
    if (envp_glib) g_strfreev(envp_glib);
    g_ptr_array_free(argv_arr, TRUE);
    umi_run_config_free(rc);

    if (!ok) {
        if (err) {
            g_set_error(err, g_quark_from_static_string("uside-run"), 4,
                        "runner error: %s", (gerr && gerr->message) ? gerr->message : "unknown");
        }
        if (gerr) g_clear_error(&gerr);
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------------------
 * Stop the active pipeline (placeholder).
 * If/when build_runner exposes a stop/kill API, call it here and clear state.
 *---------------------------------------------------------------------------*/
void
umi_run_pipeline_stop(void)
{
    /* TODO: add umi_build_runner_stop/kill when available, then set g_runner=NULL. */
    g_message("run-pipeline: stop requested (no-op; implement when runner exposes stop/kill)");
}
/*--- end of file ---*/