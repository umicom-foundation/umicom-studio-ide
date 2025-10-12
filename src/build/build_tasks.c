/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build/build_tasks.c
 * PURPOSE:
 *   Thin helpers around the build runner to execute shell commands in a
 *   controlled, cross-platform way for the IDE's task system.
 *
 * NOTES:
 *   - Matches build_runner.h signatures exactly (argv/envp are char* const*).
 *   - Keeps implementation tiny; captures/stdout routing is handled by the
 *     runner sink (see umi_build_runner_set_sink()).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                 /* gboolean, GString, GError                 */

#include "build_tasks.h"          /* this module’s header (public helpers)     */
#include "build_runner.h"         /* UmiBuildRunner API                         */

/* Execute a command with no extra args; forward any error text to 'err'.      */
gboolean
umi_run_command_simple(const char *cmd, const char *cwd, GString *out, GString *err)
{
    /* Validate cmd early.                                                     */
    if (G_UNLIKELY(!cmd || !*cmd)) {
        if (err) g_string_append(err, "umi_run_command_simple: empty command\n");
        return FALSE;
    }

    /* argv must be NULL-terminated; cast is safe (we never modify strings).   */
    char *argvv[] = { (char *)cmd, NULL };

    /* envp: NULL => inherit.                                                  */
    char * const *envp = NULL;

    /* Normalize working directory.                                            */
    const char *work_dir = (cwd && *cwd) ? cwd : ".";

    /* Create runner.                                                          */
    UmiBuildRunner *runner = umi_build_runner_new();
    if (!runner) {
        if (err) g_string_append(err, "umi_run_command_simple: runner alloc failed\n");
        return FALSE;
    }

    /* We don’t need a sink here (GUI usually wires it elsewhere).             */
    /* umi_build_runner_set_sink(runner, my_sink, my_user); */

    GError *gerr = NULL;
    gboolean ok = umi_build_runner_run(
        runner,               /* runner instance                               */
        argvv,                /* argv: { cmd, NULL }                           */
        envp,                 /* envp: inherit                                 */
        work_dir,             /* cwd                                           */
        NULL,                 /* on_exit (unused here)                         */
        NULL,                 /* user (unused here)                            */
        &gerr                 /* error details                                 */
    );

    if (!ok) {
        if (gerr) {
            if (err) g_string_append_printf(err, "runner error: %s\n",
                                            gerr->message ? gerr->message : "unknown");
            g_clear_error(&gerr);
        } else {
            if (err) g_string_append(err, "runner error (no message)\n");
        }
    } else {
        (void)out; /* Current runner version streams to sink; silence unused. */
    }

    /* If you add umi_build_runner_free() in the header later, call it here.   */
    /* umi_build_runner_free(runner); */

    return ok;
}
/*--- end of file ---*/
