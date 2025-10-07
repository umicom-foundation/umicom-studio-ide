/*
 * umi_log.c - Minimal logging shim for Umicom Studio (implementation)
 *
 * See umi_log.h for rationale and usage.
 */
#include "umi_log.h"

static int s_min_level = UMI_LOG_INFO; /* default threshold */

void umi_log_set_level(int level) {
  if (level < UMI_LOG_DEBUG) level = UMI_LOG_DEBUG;
  if (level > UMI_LOG_ERROR) level = UMI_LOG_ERROR;
  s_min_level = level;

  /* Make GLib print debug messages too when we are in DEBUG mode. */
  if (s_min_level <= UMI_LOG_DEBUG) {
    g_setenv("G_MESSAGES_DEBUG", "all", TRUE);
  }
}

static GLogLevelFlags to_glib_level(int level) {
  switch (level) {
    case UMI_LOG_DEBUG: return G_LOG_LEVEL_DEBUG;
    case UMI_LOG_INFO:  return G_LOG_LEVEL_MESSAGE;
    case UMI_LOG_WARN:  return G_LOG_LEVEL_WARNING;
    case UMI_LOG_ERROR: return G_LOG_LEVEL_CRITICAL;
    default:            return G_LOG_LEVEL_MESSAGE;
  }
}

void umi_logv(int level, const char *fmt, va_list ap) {
  if (level < s_min_level) return;
  g_logv("umicom", to_glib_level(level), fmt, ap);
}

void umi_log(int level, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  umi_logv(level, fmt, ap);
  va_end(ap);
}
