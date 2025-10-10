\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/build/build_queue.c
 * PURPOSE: Build queue callbacks
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "build_queue.h"

static void on_done(gpointer user, int code){
    (void)code; /* unused in current implementation */
    /* existing logic ... if any */
    (void)user;
}
