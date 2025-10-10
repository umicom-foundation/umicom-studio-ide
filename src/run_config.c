/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: run_config.c
 * PURPOSE: Run configuration: free helper (fix indentation warning).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include "run_config.h"

typedef struct {
    char *cwd;
    char *argv_line;
    char *env_multiline;
} UmiRunConfig;

void umi_run_config_free(UmiRunConfig *r){
    if(!r) return;
    g_free(r->cwd);
    g_free(r->argv_line);
    g_free(r->env_multiline);
    g_free(r);
}
