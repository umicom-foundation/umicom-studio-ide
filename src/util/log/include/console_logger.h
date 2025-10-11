/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/console_logger.h
 * PURPOSE: Helper to log messages into UmiOutputConsole with ANSI coloring
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONSOLE_LOGGER_H
#define UMICOM_CONSOLE_LOGGER_H

#include "../../../panes/output/include/output_console.h"

/* Append a formatted message with a level tag. */
void umi_console_log_info (UmiOutputConsole *c, const char *fmt, ...);
void umi_console_log_warn (UmiOutputConsole *c, const char *fmt, ...);
void umi_console_log_error(UmiOutputConsole *c, const char *fmt, ...);
void umi_console_log_debug(UmiOutputConsole *c, const char *fmt, ...);

#endif /* UMICOM_CONSOLE_LOGGER_H */
