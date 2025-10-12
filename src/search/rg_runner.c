/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/rg_runner.c
 * PURPOSE: Spawn ripgrep and collect its output
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <string.h>
#include "rg_runner.h"

/*---------------------------------------------------------------------------
 * umi_rg_run:
 *   Spawns the process described by argvv and captures stdout / stderr.
 *   The argvv array must be NULL-terminated. Output is appended to the
 *   provided GString buffers (they may be empty). Returns TRUE if the
 *   process was started; the process exit status is returned via
 *   *exit_status when supplied.
 *---------------------------------------------------------------------------*/
gboolean umi_rg_run(char **argvv, GString *out, GString *err, int *exit_status) {
  g_return_val_if_fail(argvv != NULL && argvv[0] != NULL, FALSE);
  g_return_val_if_fail(out != NULL && err != NULL, FALSE);

  gchar *stdout_str = NULL;
  gchar *stderr_str = NULL;
  GError *spawn_err = NULL;
  gint    status = 0;

  if (!g_spawn_sync(NULL,              /* working dir */
                    argvv,             /* argv */
                    NULL,              /* envp */
                    G_SPAWN_SEARCH_PATH,
                    NULL, NULL,        /* no child-setup */
                    &stdout_str,       /* capture stdout */
                    &stderr_str,       /* capture stderr */
                    &status,           /* exit status */
                    &spawn_err)) {
    /* Propagate error text to 'err' buffer for visibility. */
    if (spawn_err) {
      g_string_append(err, spawn_err->message);
      g_error_free(spawn_err);
    }
    g_free(stdout_str);
    g_free(stderr_str);
    return FALSE;
  }

  if (stdout_str) {
    g_string_append(out, stdout_str);
    g_free(stdout_str);
  }
  if (stderr_str) {
    g_string_append(err, stderr_str);
    g_free(stderr_str);
  }

  if (exit_status) *exit_status = status;
  return TRUE;
}
/*---------------------------------------------------------------------------*/