/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: output_pipeline.c
 * PURPOSE: Output pipeline callbacks (clean warnings).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

static void on_exit(gpointer u, int code){
    (void)u; (void)code;
}

/* If we need per-line handling later, add it back with G_GNUC_UNUSED to avoid warnings. */
