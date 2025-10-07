/*
 * umi_log.h - Minimal logging shim for Umicom Studio
 *
 * Purpose:
 *   Some source files (e.g., cli_entry.c) reference an internal logging API
 *   (umi_log_set_level, UMI_LOGD/UMI_LOGI/UMI_LOGW/UMI_LOGE). If that module
 *   isn't present in your tree, builds will fail with "undeclared identifier"
 *   errors. This header + its companion umi_log.c provide a tiny compatibility
 *   layer on top of GLib's logging so you don't have to rewrite callers.
 *
 * Usage:
 *   - Place both files under src/core/ (or any folder compiled by CMake).
 *   - No changes required to existing code that includes "umi_log.h" or uses
 *     UMI_LOG* macros; if other code doesn't include it explicitly, you can
 *     add `#include "umi_log.h"` to files that log.
 *
 * License: MIT (drop-in shim).
 */

#ifndef UMI_LOG_H
#define UMI_LOG_H

#include <stdarg.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Simple log levels compatible with typical projects */
typedef enum {
  UMI_LOG_DEBUG = 0,
  UMI_LOG_INFO  = 1,
  UMI_LOG_WARN  = 2,
  UMI_LOG_ERROR = 3
} UmiLogLevel;

/* Set the minimum level that will be emitted (default: INFO). */
void umi_log_set_level(int level);

/* Low-level helpers (usually use the macros below) */
void umi_logv(int level, const char *fmt, va_list ap);
void umi_log(int level, const char *fmt, ...) G_GNUC_PRINTF(2, 3);

/* Convenience macros used by existing code */
#define UMI_LOGD(...) umi_log(UMI_LOG_DEBUG, __VA_ARGS__)
#define UMI_LOGI(...) umi_log(UMI_LOG_INFO,  __VA_ARGS__)
#define UMI_LOGW(...) umi_log(UMI_LOG_WARN,  __VA_ARGS__)
#define UMI_LOGE(...) umi_log(UMI_LOG_ERROR, __VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMI_LOG_H */
