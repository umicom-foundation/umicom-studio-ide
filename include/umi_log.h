/*
 * umi_log.h — tiny logging shim for Umicom Studio
 *
 * Provides runtime-configurable log levels and simple macros:
 *   UMI_LOGD, UMI_LOGI, UMI_LOGW, UMI_LOGE
 *
 * This header is intentionally lightweight and GLib-only.
 * It maps to g_log() with domain "umicom".
 */
#pragma once
#include <glib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UMI_LOG_DEBUG = 0,
  UMI_LOG_INFO  = 1,
  UMI_LOG_WARN  = 2,
  UMI_LOG_ERROR = 3
} UmiLogLevel;

void umi_log_set_level(int level);
int  umi_log_get_level(void);
void umi_log_log(int level, const char *fmt, ...) G_GNUC_PRINTF(2,3);

/* Convenience macros */
#define UMI_LOGD(...) do { if (umi_log_get_level() <= UMI_LOG_DEBUG) umi_log_log(UMI_LOG_DEBUG, __VA_ARGS__); } while(0)
#define UMI_LOGI(...) do { if (umi_log_get_level() <= UMI_LOG_INFO)  umi_log_log(UMI_LOG_INFO,  __VA_ARGS__); } while(0)
#define UMI_LOGW(...) do { if (umi_log_get_level() <= UMI_LOG_WARN)  umi_log_log(UMI_LOG_WARN,  __VA_ARGS__); } while(0)
#define UMI_LOGE(...) do { if (umi_log_get_level() <= UMI_LOG_ERROR) umi_log_log(UMI_LOG_ERROR, __VA_ARGS__); } while(0)

#ifdef __cplusplus
}
#endif
