/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/git/git_integration.c
 * PURPOSE: Implementation of Git helpers (child process + capture)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "git_integration.h"   /* Our public declarations */
#include <string.h>            /* strlen */
#include <stdio.h>             /* snprintf */

/* Internal helper: build argv and spawn a git process synchronously.
 *
 * - dir: if non-NULL, run git with "-C dir" to select repo/work-tree.
 * - argv_tail: NULL-terminated vector with the git subcommand and args,
 *              e.g. {"status","--porcelain",NULL}
 * - out_stdout: (optional) receives newly-allocated UTF-8 from child stdout.
 * - err: standard GLib error out.
 *
 * Returns TRUE on exit status 0. Even on failure, *out_stdout is set to NULL.
 */
static gboolean
run_git (const char   *dir,
         const char  **argv_tail,
         char        **out_stdout,
         GError      **err)
{
  /* Safety: default outputs */
  if (out_stdout) *out_stdout = NULL;

  /* Construct the argv vector: ["git", ("-C", dir)? , tail..., NULL] */
  GPtrArray *argv = g_ptr_array_new_with_free_func (NULL /* items are const */);
  g_ptr_array_add (argv, (gpointer) "git");

  if (dir && *dir) {
    g_ptr_array_add (argv, (gpointer) "-C");
    g_ptr_array_add (argv, (gpointer) dir);
  }

  for (const char **p = argv_tail; p && *p; ++p)
    g_ptr_array_add (argv, (gpointer) *p);

  g_ptr_array_add (argv, NULL); /* NULL-terminate */

  /* Spawn synchronously; capture stdout/stderr. Note:
   *  - We do not use a shell, so args are not re-parsed (safer).
   *  - GLib handles Windows process creation under the hood.
   */
  gchar  *child_stdout = NULL;
  gchar  *child_stderr = NULL;
  gint    exit_status  = 0;

  gboolean ok = g_spawn_sync (/* working_dir */ NULL,
                              (gchar **) argv->pdata,
                              /* envp */ NULL,
                              G_SPAWN_SEARCH_PATH, /* find "git" in PATH */
                              /* child_setup */ NULL, /* data */ NULL,
                              &child_stdout,
                              &child_stderr,
                              &exit_status,
                              err);

  g_ptr_array_free (argv, TRUE);

  if (!ok) {
    /* g_spawn_sync has already filled GError for us. */
    g_free (child_stdout);
    g_free (child_stderr);
    return FALSE;
  }

  /* Join non-zero exit into a GLib error so callers have context. */
  if (exit_status != 0) {
    g_set_error (err,
                 g_quark_from_static_string ("umi-git"),
                 exit_status,
                 "git exited with status %d: %s",
                 exit_status,
                 child_stderr ? child_stderr : "no stderr");
    g_free (child_stdout);
    g_free (child_stderr);
    return FALSE;
  }

  /* Success: hand stdout to caller as-is. */
  if (out_stdout) *out_stdout = child_stdout; else g_free (child_stdout);
  g_free (child_stderr);
  return TRUE;
}

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

char *
umi_git_status (const char *dir, GError **err)
{
  /* Ask git for a machine-friendly status. */
  const char *tail[] = { "status", "--porcelain", NULL };
  char *out = NULL;

  if (!run_git (dir, tail, &out, err)) {
    /* On failure, out remains NULL. */
    return NULL;
  }

  /* Ensure UTF-8 (GLib spawns return bytes; Git prints UTF-8 by default). */
  if (!g_utf8_validate (out, -1, NULL)) {
    g_set_error (err, g_quark_from_static_string ("umi-git"), 1,
                 "git output is not valid UTF-8");
    g_clear_pointer (&out, g_free);
    return NULL;
  }

  return out; /* Caller must g_free() */
}

gboolean
umi_git_add_all (const char *dir, GError **err)
{
  const char *tail[] = { "add", "-A", NULL };
  return run_git (dir, tail, NULL, err);
}

gboolean
umi_git_commit (const char *dir, const char *message, GError **err)
{
  if (!message || !*message) {
    g_set_error (err, g_quark_from_static_string ("umi-git"), 1,
                 "commit message is empty");
    return FALSE;
  }

  /* We pass message as a single argv token; no shell quoting required. */
  const char *tail[] = { "commit", "-m", message, NULL };
  return run_git (dir, tail, NULL, err);
}
