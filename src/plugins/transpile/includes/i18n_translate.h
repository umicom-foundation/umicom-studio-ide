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

/**
 * umi_translate_text:
 * @input_text: NUL-terminated UTF-8 text to translate. Must not be NULL.
 * @src_lang:   ISO-ish language code of the input (e.g. "en"). May be NULL to auto-detect.
 * @dst_lang:   Target language code (e.g. "ar"). Must not be NULL/empty.
 * @cfg:        LLM configuration (endpoint/provider/model/key). Must not be NULL.
 * @errbuf:     Caller-provided buffer for a human-readable error message.
 * @errcap:     Size of @errbuf in bytes.
 *
 * Returns newly allocated UTF-8 string on success (g_free() when done),
 * or NULL on error (errbuf will contain a short message).
 */
gchar *umi_translate_text(const char *input_text,
                          const char *src_lang,
                          const char *dst_lang,
                          const UmiLlmCfg *cfg,
                          char *errbuf,
                          gsize errcap);

G_END_DECLS

#endif /* UMI_TRANSLATE_I18N_H */
/*--- end of file ---*/