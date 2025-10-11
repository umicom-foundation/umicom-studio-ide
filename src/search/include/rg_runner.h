/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/rg_runner.h
 * PURPOSE: Minimal ripgrep subprocess spawner
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RG_RUNNER_H
#define UMICOM_RG_RUNNER_H

#include <gio/gio.h>

typedef struct {
  gchar *pattern;
  gchar *root;
  gboolean ignore_case;
} UmiRgOpts;

GSubprocess *umi_rg_spawn(const UmiRgOpts *o, GError **err);

#endif /* UMICOM_RG_RUNNER_H */
