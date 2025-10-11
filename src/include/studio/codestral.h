/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: include/studio/codestral.h
 * PURPOSE: Codestral FIM function prototypes for IDE integration.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_CODESTRAL_H
#define UMICOM_STUDIO_CODESTRAL_H
#ifdef __cplusplus
extern "C" {
#endif

/* FIM request. Returns 0 on success and writes raw JSON into 'out' */
int studio_codestral_fim(const char *api_key,
                         const char *base_url,
                         const char *model,
                         const char *prefix,
                         const char *suffix,
                         int max_tokens,
                         double temperature,
                         char *out, unsigned long outsz);

#ifdef __cplusplus
}
#endif
#endif
