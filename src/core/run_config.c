\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/run_config.c
 * PURPOSE: Run configuration helpers
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include "run_config.h"

void umi_run_config_free(UmiRunConfig *r){
  if(!r) return;
  g_free(r->cwd);
  g_free(r->argv_line);
  g_free(r->env_multiline);
  g_free(r);
}
