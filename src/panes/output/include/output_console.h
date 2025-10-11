/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/output_console.h
 * PURPOSE: Rich output console with ANSI + filter chain
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OUTPUT_CONSOLE_H
#define UMICOM_OUTPUT_CONSOLE_H

#include <gtk/gtk.h>
#include "../../../util/sys/include/ansi_color.h"
#include "output_filters.h"

typedef struct _UmiOutputConsole UmiOutputConsole;

UmiOutputConsole* umi_output_console_new(void);
GtkWidget*        umi_output_console_widget(UmiOutputConsole *c);

/* Append raw line; ANSI escapes are parsed; filters can add metadata. */
void umi_output_console_append_line(UmiOutputConsole *c, const char *line);

/* Clear console. */
void umi_output_console_clear(UmiOutputConsole *c);

/* Access underlying filter chain (to add custom filters). */
UmiOutChain* umi_output_console_chain(UmiOutputConsole *c);

#endif /* UMICOM_OUTPUT_CONSOLE_H */
