/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: msys_env.c
 * PURPOSE: MSYS2 helpers (avoid unused variable warning).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

const char* umi_msys_path_hint(void){
    const char *msys = g_getenv("MSYS2_PATH_TYPE");
    (void)msys;
    return g_getenv("PATH");
}
