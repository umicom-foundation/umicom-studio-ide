/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/core/include/run_config.h
 * PURPOSE:
 *   Minimal, consistent run configuration used by the run pipeline.
 *
 * DESIGN:
 *   - Header stays GLib-only.
 *   - Functions return newly-allocated argv/envp vectors (NULL-terminated).
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUN_CONFIG_H
#define UMICOM_RUN_CONFIG_H

#include <glib.h>

typedef struct {
  char       *exe;    /* e.g., "clang" or absolute path.                   */
  GPtrArray  *args;   /* array of (char*) not including argv[0]; may be NULL. */
  GPtrArray  *env;    /* array of "KEY=VALUE" (char*); may be NULL.        */
  char       *cwd;    /* optional working directory; NULL => inherit.      */
} UmiRunConfig;

/* Load config (from defaults/env/settings). Safe to stub; never returns NULL. */
UmiRunConfig *umi_run_config_load(void);

/* Convert to argv vector suitable for g_subprocess / runner. */
char **umi_run_config_to_argv(const UmiRunConfig *rc, int *out_argc);

/* Convert to envp vector (NULL-terminated). NULL => inherit parent env. */
char **umi_run_config_to_envp(const UmiRunConfig *rc);

/* Free all fields + the struct itself. */
void umi_run_config_free(UmiRunConfig *rc);

#endif /* UMICOM_RUN_CONFIG_H */
/*--- end of file ---*/