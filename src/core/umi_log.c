/*
 * umi_log.c — implementation for tiny logging shim
 */
#include "umi_log.h"
#include <glib.h>
#include <stdarg.h>
#include <stdio.h>

static volatile gint s_level = UMI_LOG_INFO; /* default INFO */

void umi_log_set_level(int level) {
  if (level < UMI_LOG_DEBUG) level = UMI_LOG_DEBUG;
  if (level > UMI_LOG_ERROR) level = UMI_LOG_ERROR;
  g_atomic_int_set(&s_level, level);
}

int umi_log_get_level(void) {
  return g_atomic_int_get(&s_level);
}

static GLogLevelFlags to_glib(int lvl){
  switch (lvl){
    case UMI_LOG_DEBUG: return G_LOG_LEVEL_DEBUG;
    case UMI_LOG_INFO:  return G_LOG_LEVEL_MESSAGE;
    case UMI_LOG_WARN:  return G_LOG_LEVEL_WARNING;
    default:            return G_LOG_LEVEL_CRITICAL;
  }
}

void umi_log_log(int level, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  gchar *msg = g_strdup_vprintf(fmt, ap);
  va_end(ap);
  g_log("umicom", to_glib(level), "%s", msg);
  g_free(msg);
}
