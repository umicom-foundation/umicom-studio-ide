/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_sources.h
 *
 * PURPOSE:
 *   Create a Studio source record for AI citation and retrieval displays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Source construction stays independent of the UI so chat, retrieval and future editor annotations can share attribution data.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_SOURCES_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_SOURCES_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_ai_source(const char *source_id, const char *title, const char *uri, UmiAiSource *source);

#ifdef __cplusplus
}
#endif

#endif
