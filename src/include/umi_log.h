/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: include/umi_log.h
 * PURPOSE: Lightweight logging interface (shim) used by CLI and other modules
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMI_LOG_H
#define UMI_LOG_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal logging levels (match names used by cli_entry.c) */
typedef enum {
    UMI_LOG_ERROR = 0,
    UMI_LOG_WARN  = 1,
    UMI_LOG_INFO  = 2,
    UMI_LOG_DEBUG = 3
} UmiLogLevel;

/* Runtime control of verbosity */
void umi_log_set_level(UmiLogLevel lvl);
UmiLogLevel umi_log_get_level(void);

/* Internal helper; prefer the UMI_LOG* macros below */
void umi_log_log(UmiLogLevel lvl, const char *file, int line, const char *fmt, ...);

/* Convenience macros expected by existing code */
#define UMI_LOGE(fmt, ...) umi_log_log(UMI_LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define UMI_LOGW(fmt, ...) umi_log_log(UMI_LOG_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define UMI_LOGI(fmt, ...) umi_log_log(UMI_LOG_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define UMI_LOGD(fmt, ...) umi_log_log(UMI_LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* UMI_LOG_H */
