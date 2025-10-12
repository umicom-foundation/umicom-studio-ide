/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_tasks.c
 * PURPOSE: Thin helpers around the build runner to execute shell commands in a
 *          controlled, cross-platform way for the IDE's task system.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------
 * Quick Start / Notes:
 * - This wrapper matches the current build_runner.h signature:
 *
 *     gboolean umi_build_runner_run(
 *         UmiBuildRunner *br,
 *         char * const *argv,   // argv[0] = cmd, NULL-terminated
 *         char * const *envp,   // optional environment vector (NULL = inherit)
 *         const char *cwd,      // working directory (e.g., ".")
 *         UmiBuildExitCb on_exit,
 *         gpointer user,
 *         GError **err          // failure details (NULL if not needed)
 *     );
 *
 * - We do NOT capture stdout/stderr here because the runner API does not expose
 *   capture buffers in its parameters. If needed later, we can add a helper
 *   that reads GSubprocess pipes inside the runner and returns text.
 * - Pure C (C17) + GLib types; zero GtkBuilder/GResource/XML.
 * - Keep warnings clean (-Wall -Wextra -Wpedantic). Avoid unused locals.
 *---------------------------------------------------------------------------*/

#include <glib.h>               /* gboolean, GString, GError, g_clear_error    */
#include <string.h>             /* strlen (not strictly needed but common)     */

#include "build_tasks.h"        /* public declaration for umi_run_command_simple */
#include "build_runner.h"       /* UmiBuildRunner ctor + run, UmiBuildExitCb     */

/*-----------------------------------------------------------------------------
 * Function: umi_run_command_simple
 *
 * PURPOSE:
 *   Execute a single command with no extra arguments, optionally in a specific
 *   working directory. On failure, forward any GError message into 'err'
 *   (GString) so callers can surface a textual diagnostic.
 *
 * PARAMETERS:
 *   cmd  - (in)  Executable path or command name (argv[0]); must be non-NULL.
 *   cwd  - (in)  Working directory; if NULL/empty, defaults to ".".
 *   out  - (out) Optional stdout text sink (currently unused here).
 *   err  - (out) Optional error text sink; receives GError message on failure.
 *
 * RETURNS:
 *   TRUE on success, FALSE on failure (see 'err' for details if provided).
 *---------------------------------------------------------------------------*/
gboolean
umi_run_command_simple(const char *cmd, const char *cwd, GString *out, GString *err)
{
    /* Validate 'cmd' early to avoid undefined behavior further down. */
    if (G_UNLIKELY(cmd == NULL || *cmd == '\0')) {
        if (err) g_string_append(err, "umi_run_command_simple: empty command\n");
        return FALSE;
    }

    /* Build argv for the runner:
     *  - argv[0] must be the command itself,
     *  - argv must be NULL-terminated,
     *  - type must be 'char * const *'.
     * We cast away const on 'cmd' because we do not modify it; the runner treats
     * argv as read-only (execv-style). */
    char *argvv[] = { (char *)cmd, NULL };

    /* Environment vector:
     *  - NULL means "inherit current process environment".
     *  - If you later want a custom environment, build a char* vector like:
     *      char *envp[] = { "VAR1=VALUE1", "VAR2=VALUE2", NULL };
     *    and pass 'envp'. */
    char * const *envp = NULL;

    /* Normalise the working directory; default to "." if not provided. */
    const char *work_dir = (cwd && *cwd) ? cwd : ".";

    /* Create the runner instance (constructor takes no parameters). */
    UmiBuildRunner *runner = umi_build_runner_new();
    if (G_UNLIKELY(runner == NULL)) {
        if (err) g_string_append(err, "umi_run_command_simple: failed to create runner\n");
        return FALSE;
    }

    /* Optional exit callback and user data — unused in this simple helper. */
    UmiBuildExitCb on_exit = NULL;
    gpointer       user    = NULL;

    /* Prepare GError** for detailed failure reporting from the runner. */
    GError *gerr = NULL;

    /* Call the runner with the correct parameter order and types:
     *   (runner, argv, envp, cwd, on_exit, user, &gerr) */
    gboolean ok = umi_build_runner_run(
        runner,     /* runner instance                                          */
        argvv,      /* argv: { cmd, NULL }                                      */
        envp,       /* envp: NULL => inherit                                    */
        work_dir,   /* cwd                                                      */
        on_exit,    /* optional exit callback (unused here)                      */
        user,       /* user data (unused here)                                   */
        &gerr       /* failure details (NULL if success)                         */
    );

    /* If the runner reported an error, forward its message into 'err' so
     * callers that expect textual diagnostics receive them. */
    if (!ok) {
        if (gerr) {
            if (err) g_string_append_printf(err, "runner error: %s\n",
                                            gerr->message ? gerr->message : "unknown");
            g_clear_error(&gerr); /* frees and NULLs the pointer */
        } else {
            if (err) g_string_append(err, "runner error: (no GError message)\n");
        }
    } else {
        /* Success: the current runner API does not expose stdout capture; mark
         * 'out' intentionally unused to keep -Wunused-parameter clean. */
        (void)out;
    }

    /* If/when a destructor exists (e.g., umi_build_runner_free), call it here.
     * We avoid assuming it to keep this file aligned with your current headers. */
    /* umi_build_runner_free(runner); */

    return ok;
}
/*--- end of file ---*/