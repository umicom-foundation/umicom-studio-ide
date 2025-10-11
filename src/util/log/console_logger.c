/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/console_logger.c
 * PURPOSE: Implementation of console logger
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "console_logger.h"
#include <stdarg.h>

static void vpost(UmiOutputConsole *c, const char *sev, const char *color, const char *fmt, va_list ap){
  if(!c || !fmt) return;
  gchar *msg = g_strdup_vprintf(fmt, ap);
  gchar *line = g_strdup_printf("%s[%s]%s %s",
    color?color:"", sev?sev:"info", color?"\x1b[0m":"", msg?msg:"");
  umi_output_console_append_line(c, line);
  g_free(line);
  g_free(msg);
}

#define GEN(NAME, SEV, COLOR) \
void NAME(UmiOutputConsole *c, const char *fmt, ...){ \
  va_list ap; va_start(ap, fmt); vpost(c, SEV, COLOR, fmt, ap); va_end(ap); \
}

GEN(umi_console_log_info,  "info",  "\x1b[36m")
GEN(umi_console_log_warn,  "warn",  "\x1b[33m")
GEN(umi_console_log_error, "error", "\x1b[31m")
GEN(umi_console_log_debug, "debug", "\x1b[90m")
