/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/umi_log.c
 * PURPOSE: Implementation of lightweight logging shim (no external deps)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "umi_log.h"
#include <time.h>

static UmiLogLevel g_level = UMI_LOG_INFO;

void umi_log_set_level(UmiLogLevel lvl) { g_level = lvl; }
UmiLogLevel umi_log_get_level(void) { return g_level; }

static const char* level_tag(UmiLogLevel lvl) {
    switch(lvl){
        case UMI_LOG_ERROR: return "ERROR";
        case UMI_LOG_WARN:  return "WARN";
        case UMI_LOG_INFO:  return "INFO";
        default:            return "DEBUG";
    }
}

void umi_log_log(UmiLogLevel lvl, const char *file, int line, const char *fmt, ...) {
    if(lvl > g_level) return;

    /* ISO timestamp */
    time_t t = time(NULL);
    struct tm tmv;
#if defined(_WIN32)
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tmv);

    fprintf(stderr, "[%s] %-5s %s:%d: ", ts, level_tag(lvl), file ? file : "?", line);

    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
}
