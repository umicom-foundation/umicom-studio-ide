\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/output_pipeline.c
 * PURPOSE: Process output from tools
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>

static void on_exit(gpointer u, int code){
  (void)u; (void)code; /* not used yet */
}

/* Removed unused 'on_line' stub to silence -Wunused-function */
