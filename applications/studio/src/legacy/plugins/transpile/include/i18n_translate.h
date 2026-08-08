/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/include/i18n_translate.h
 * PURPOSE:
 *   Public API for simple LLM-powered text translation used by the
 *   "Transpile" demo/plugin.
 *
 * DESIGN:
 *   - Header-only contract: no GLib types leak in the signature beyond
 *     what is already used widely (gsize).
 *   - Error reporting via (errbuf, errcap) to avoid GError** in public API.
 *   - Does not depend on UI; pure core utility callable from anywhere.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_TRANSLATE_I18N_H
#define UMI_TRANSLATE_I18N_H

#include <glib.h>     /* gsize */
#include <llm.h>      /* UmiLlmCfg */

G_BEGIN_DECLS

gchar *umi_translate_text(const char *input_text,
                          const char *src_lang,
                          const char *dst_lang,
                          const UmiLlmCfg *cfg,
                          char *errbuf,
                          gsize errcap);

G_END_DECLS
#endif /* UMI_TRANSLATE_I18N_H */
/*--- end of file ---*/
