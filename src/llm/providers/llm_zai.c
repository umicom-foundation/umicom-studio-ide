/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/providers/llm_zai.c
 * PURPOSE: Z.ai provider stub (kept minimal until live API keys & endpoints)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-11 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

/*
 * NOTE:
 * - The previous file contained a UTF-8 BOM and non-ASCII punctuation (em-dashes)
 *   which can trigger compiler/toolchain warnings on some setups.
 * - It also included a non-existent path <umicom/llm.h> in this tree snapshot.
 *   We avoid wrong includes here and keep a clean, compilable stub until the
 *   provider contract is finalized.
 *
 * This stub exists so the project builds without warnings. When the live Z.ai
 * integration is ready (token + base URL + model names), add:
 *   - request builder (JSON)
 *   - HTTP call via umi_http_post_json / umi_http_post_stream
 *   - response parser (choices / text)
 *   - error surface via GError
 */

/* Example placeholder API (commented out until finalized)
gboolean umi_llm_zai_chat(const gchar *system,
                          const gchar *user,
                          gchar      **out_text,
                          gchar       *errbuf, gsize errcap)
{
  (void)system; (void)user; (void)out_text; (void)errbuf; (void)errcap;
  // TODO: implement using umi_http_post_json once Z.ai endpoints settle.
  return FALSE;
}
*/
/*---------------------------------------------------------------------------*/