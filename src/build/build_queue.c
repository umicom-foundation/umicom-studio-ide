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
#include <build_queue.h>

/* If this stays for future wiring, silence the warning portably */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((unused))
#endif
static void on_done(void *user, int code) { (void)user; (void)code; }
/*-----------------------------------------------------------------------------
 * Lifecycle
 *---------------------------------------------------------------------------*/
