#ifndef UMICOM_TRANSPILE_H
#define UMICOM_TRANSPILE_H
/*---------------------------------------------------------------------------
* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
*---------------------------------------------------------------------------*/
#include <glib.h>
#include <stdbool.h>
typedef struct {
  gchar *src_lang;
  gchar *dst_lang;
  gboolean keep_comments;
  gboolean add_explanations;
} UmiTranspileOpts;
bool umi_transpile_code(const gchar *source,
                        const UmiTranspileOpts *opts,
                        gchar **out_text,
                        gchar *errbuf, gsize errcap);
#endif
