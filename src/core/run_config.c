/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/run_config.c
 * PURPOSE:
 *   Implementation for the minimal run configuration used by the pipeline.
 *
 * NOTES:
 *   This version is intentionally conservative: it loads sensible defaults
 *   and reads a few environment variables commonly used during development.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "run_config.h"
#include <string.h>

static char *dup(const char *s) { return s ? g_strdup(s) : NULL; }

UmiRunConfig *umi_run_config_load(void) {
  UmiRunConfig *rc = g_new0(UmiRunConfig, 1);
  rc->args = g_ptr_array_new_with_free_func(g_free);
  rc->env  = g_ptr_array_new_with_free_func(g_free);

  /* Defaults: exe from env UMI_RUN_EXE or "clang" as a generic placeholder. */
  const char *exe = g_getenv("UMI_RUN_EXE");
  rc->exe = dup(exe && *exe ? exe : "clang");

  /* Optional working directory from env; otherwise inherit. */
  const char *cwd = g_getenv("UMI_RUN_CWD");
  rc->cwd = dup((cwd && *cwd) ? cwd : NULL);

  /* Optional extra args from env UMI_RUN_ARGS (space-separated, naive split). */
  const char *args = g_getenv("UMI_RUN_ARGS");
  if (args && *args) {
    g_auto(GStrv) toks = g_strsplit_set(args, " \t", -1);
    for (gint i = 0; toks && toks[i]; ++i) {
      if (*toks[i]) g_ptr_array_add(rc->args, g_strdup(toks[i]));
    }
  }

  /* Optional env additions from UMI_RUN_ENV ("K=V;K2=V2"). */
  const char *env = g_getenv("UMI_RUN_ENV");
  if (env && *env) {
    g_auto(GStrv) pairs = g_strsplit(env, ";", -1);
    for (gint i = 0; pairs && pairs[i]; ++i) {
      if (*pairs[i] && strchr(pairs[i], '=')) {
        g_ptr_array_add(rc->env, g_strdup(pairs[i]));
      }
    }
  }

  return rc;
}

char **umi_run_config_to_argv(const UmiRunConfig *rc, int *out_argc) {
  g_return_val_if_fail(rc && rc->exe, NULL);

  /* argv = [exe, args..., NULL] */
  guint n = (rc->args ? rc->args->len : 0);
  char **argv = g_new0(char*, n + 2);
  argv[0] = g_strdup(rc->exe);
  for (guint i = 0; i < n; ++i) argv[i+1] = g_strdup((char*)rc->args->pdata[i]);
  if (out_argc) *out_argc = (int)(n + 1);
  return argv; /* NULL-terminated */
}

char **umi_run_config_to_envp(const UmiRunConfig *rc) {
  if (!rc || !rc->env || rc->env->len == 0) return NULL; /* inherit */
  guint n = rc->env->len;
  char **envp = g_new0(char*, n + 1);
  for (guint i = 0; i < n; ++i) envp[i] = g_strdup((char*)rc->env->pdata[i]);
  return envp; /* NULL-terminated */
}

void umi_run_config_free(UmiRunConfig *rc) {
  if (!rc) return;
  g_free(rc->exe);
  g_free(rc->cwd);
  if (rc->args) g_ptr_array_free(rc->args, TRUE);
  if (rc->env)  g_ptr_array_free(rc->env, TRUE);
  g_free(rc);
}
/*--- end of file ---*/