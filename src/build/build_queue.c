/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: build_queue.c
 * PURPOSE: Build queue helpers (silence unused params).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

static void on_done(gpointer user, int code){
    (void)user; (void)code;
    /* No-op callback for now; hook build completion here */
}
