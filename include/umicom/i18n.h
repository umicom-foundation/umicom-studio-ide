#ifndef UMICOM_I18N_H
#define UMICOM_I18N_H
/*---------------------------------------------------------------------------
* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
*---------------------------------------------------------------------------*/
#include <glib.h>
#include <stdbool.h>
typedef struct {
  gchar *src_locale;
  gchar *dst_locale;
  gboolean formal;
} UmiI18nOpts;
bool umi_translate_text(const gchar *input,
                        const UmiI18nOpts *opts,
                        gchar **out_text,
                        gchar *errbuf, gsize errcap);
#endif
