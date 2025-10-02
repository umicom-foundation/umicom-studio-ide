/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/log.c
 * PURPOSE: Implementation for log verbosity toggling
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "log.h"

static gboolean g_debug_enabled = FALSE;

static void handler(const gchar *log_domain, GLogLevelFlags log_level,
                    const gchar *message, gpointer user_data){
  (void)user_data;
  if(!(log_level & G_LOG_LEVEL_DEBUG) || g_debug_enabled){
    g_log_default_handler(log_domain, log_level, message, NULL);
  }
}

void umi_log_set_verbosity(gboolean debug_enabled){
  g_debug_enabled = debug_enabled;
  g_log_set_default_handler(handler, NULL);
}
