/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/argv.h
 * PURPOSE: Simple CLI parsing to set workspace root and open file
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ARGV_H
#define UMICOM_ARGV_H

#include <gtk/gtk.h>

/* Parse args:
 *   --root <dir>     set workspace root
 *   --open <file>    open file on start
 *   --theme <name>   force theme: light|dark
 *   Returns TRUE if recognized switches were processed (argv mutated). */
gboolean umi_argv_apply(GtkApplication *app, int *argc, char ***argv);

#endif /* UMICOM_ARGV_H */
