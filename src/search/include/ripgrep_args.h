/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ripgrep_args.h
 * PURPOSE: Build argv for ripgrep based on UI toggles
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RIPGREP_ARGS_H
#define UMICOM_RIPGREP_ARGS_H

#include <glib.h>

typedef struct {
  gboolean regex;
  gboolean match_case;
  const char *query;
  const char *folder;
} UmiRgCfg;

GPtrArray *umi_rg_build_argv(const UmiRgCfg *cfg);

#endif /* UMICOM_RIPGREP_ARGS_H */
