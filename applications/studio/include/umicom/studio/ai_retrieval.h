/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_retrieval.h
 *
 * PURPOSE:
 *   Expose deterministic local document chunking and retrieval for Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This reference retrieval path works without an embedding model, providing a reliable fallback for source-aware AI features.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_RETRIEVAL_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_RETRIEVAL_H

#include <stddef.h>
#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_studio_ai_retrieve(const char *document_id,
                              const char *document_text,
                              const char *query,
                              UmiAiRetrievalResult *results,
                              size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
