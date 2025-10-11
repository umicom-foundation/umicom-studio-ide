\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/msys_env.c
 * PURPOSE: MSYS/MinGW environment helpers
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>

const char* umi_msys_path_hint(void){
  const char *msys = g_getenv("MSYS2_PATH_TYPE");
  (void)msys; /* currently unused; reserved for future behavior tweaks */
  return g_getenv("PATH");
}
