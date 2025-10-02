/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/git_integration.c
 * PURPOSE: Implementation of Git helpers (child process + capture)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "git_integration.h"

static gchar *run(const char *cwd, char * const *argv, GError **err){
  GSubprocess *p = g_subprocess_newv((const gchar * const*)argv,
      G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE, err);
  if(!p) return NULL;
  if(cwd){
    /* In a full implementation we would use GSubprocessLauncher to set cwd.
     * To keep the demo simple, we rely on being launched from the repo root. */
  }
  gchar *out=NULL, *errtxt=NULL; g_subprocess_communicate_utf8(p, NULL, NULL, &out, &errtxt, err);
  if(errtxt && *errtxt){ /* If git prints an error to stderr, show it in the output. */
    gchar *both = g_strconcat(out?out:"", errtxt, NULL); g_free(out); out = both;
  }
  g_free(errtxt);
  g_object_unref(p);
  return out;
}

gchar *umi_git_status(const char *cwd, GError **err){
  char * const argv[] = { "git", "status", "--porcelain", NULL };
  return run(cwd, argv, err);
}

gchar *umi_git_add_all(const char *cwd, GError **err){
  char * const argv[] = { "git", "add", "-A", NULL };
  return run(cwd, argv, err);
}

gchar *umi_git_commit(const char *cwd, const char *message, GError **err){
  char * const argv[] = { "git", "commit", "-m", (char*)(message?message:"update"), NULL };
  return run(cwd, argv, err);
}
