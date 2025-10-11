/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/options.h
 * PURPOSE: Runtime options parsing (GOptionContext)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPTIONS_H
#define UMICOM_OPTIONS_H

#include <glib.h>

typedef struct {
  gboolean verbose;
  gint     log_level;     /* 0=DEBUG..3=ERROR */
  gchar   *workspace;
  gboolean headless;
} UmiOptions;

UmiOptions *umi_options_parse(int *argc, char ***argv, GError **err);
void        umi_options_free(UmiOptions *o);

#endif /* UMICOM_OPTIONS_H */
