/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/run_config.h
 * PURPOSE: JSON-backed run configuration utilities
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RUN_CONFIG_H
#define UMICOM_RUN_CONFIG_H

#include <glib.h>

typedef struct {
  gchar *cwd;          /* working directory */
  gchar *argv_line;    /* space-separated argv (TODO: quoting) */
  gchar *env_multiline;/* KEY=VALUE per line */
} UmiRunConfig;

UmiRunConfig *umi_run_config_load(void);
gboolean      umi_run_config_save(const UmiRunConfig *rc);
void          umi_run_config_free(UmiRunConfig *rc);

/* Convert argv_line into a GPtrArray of gchar* (NULL-terminated). */
GPtrArray    *umi_run_config_to_argv(const UmiRunConfig *rc);

/* Convert env_multiline into envp vector (NULL-terminated). */
gchar       **umi_run_config_to_envp(const UmiRunConfig *rc);

#endif /* UMICOM_RUN_CONFIG_H */
