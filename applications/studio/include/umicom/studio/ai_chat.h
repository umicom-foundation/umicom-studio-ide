/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_chat.h
 *
 * PURPOSE:
 *   Provide a small Studio chat use case over the shared Framework AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The chat adapter assembles a Framework request; it does not know how the selected model is hosted.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_CHAT_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_CHAT_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_ai_chat(UmiStudioAiPlatform *platform,
                             const char *prompt,
                             UmiAiResponse *response);

#ifdef __cplusplus
}
#endif

#endif
