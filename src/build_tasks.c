/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/build_tasks.c
 * PURPOSE: Minimal build task runner glue for launching external build tools
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <gtk/gtk.h>

/* Project headers */
#include "app.h"            /* UmiApp, umi_app_handle (if you want to use it)   */
#include "build_runner.h"   /* UmiBuildRunner, umi_build_runner_new/_run        */

/* Forward declare to avoid dragging UI types if you don't need them yet.
 * If you later want to display runner output, replace NULL below with a real
 * UmiOutputPane* (for example from your editor) and include its header. */
typedef struct _UmiOutputPane UmiOutputPane;

/* Optional: simple exit callback signature is defined by build_runner.h.
 * We keep it NULL in the call to umi_build_runner_run() for now. */

/*-----------------------------------------------------------------------------
 * Helper: run a shell-style command line via UmiBuildRunner
 * This is intentionally small and self-contained:
 *  - Parses cmdline into argv with g_shell_parse_argv
 *  - Creates a runner (currently with NULL output sink)
 *  - Invokes umi_build_runner_run with correct 6 parameters
 *  - Frees resources
 * Returns: TRUE on successful launch, FALSE on failure (and logs via g_warning)
 *---------------------------------------------------------------------------*/
static gboolean
umi_run_command_simple(const char *cwd, const char *cmdline)
{
  g_return_val_if_fail(cmdline != NULL, FALSE);

  gboolean ok = FALSE;
  GError  *perr = NULL;
  int      argc = 0;
  char   **argv = NULL;

  if (!g_shell_parse_argv(cmdline, &argc, &argv, &perr)) {
    g_warning("build_tasks: parse error: %s", perr ? perr->message : "unknown");
    if (perr) g_error_free(perr);
    return FALSE;
  }

  /* Create runner; pass NULL for output pane for now. */
  UmiBuildRunner *runner = umi_build_runner_new(NULL);

  /* Launch with the full 6-parameter API. We don't need an on-exit callback
   * yet, so we pass NULLs for on_exit/user, and we pass &perr for errors. */
  ok = umi_build_runner_run(runner,
                            cwd ? cwd : ".",     /* cwd (use "." if NULL)      */
                            (char * const *)argv,/* argv vector                */
                            NULL,                 /* on_exit callback (optional)*/
                            NULL,                 /* user data                   */
                            &perr);               /* error out                   */

  if (!ok) {
    g_warning("build_tasks: launch failed: %s", perr ? perr->message : "unknown");
  }

  if (runner)        umi_build_runner_free(runner);
  if (argv)          g_strfreev(argv);
  if (perr)          g_error_free(perr);
  return ok;
}

/*-----------------------------------------------------------------------------
 * Public convenience function you can call from menus/actions.
 * Example usage:
 *    umi_build_tasks_run(app, ".", "ninja -C build-ninja -j1");
 *---------------------------------------------------------------------------*/
void
umi_build_tasks_run(GtkApplication *app, const char *cwd, const char *cmdline)
{
  (void)app; /* Not needed right now. If you want to reach your UI, you can:
              *   UmiApp *ua = umi_app_handle(app);
              * and then use ua->ed / ua->status, etc. */

  if (!umi_run_command_simple(cwd, cmdline)) {
    g_warning("umi_build_tasks_run: command failed to start: %s", cmdline ? cmdline : "(null)");
  }
}

/*-----------------------------------------------------------------------------
 * (Optional) register a couple of sample actions; safe to leave unused.
 * Hook this up if you want to bind to app actions later.
 *---------------------------------------------------------------------------*/
void
umi_build_tasks_register_actions(GtkApplication *app)
{
  (void)app;
  /* Example if you later want actions:
   *   g_action_map_add_action_entries(G_ACTION_MAP(app), entries, G_N_ELEMENTS(entries), app);
   * For now, this file’s primary purpose is to provide a correct runner call.
   */
}
