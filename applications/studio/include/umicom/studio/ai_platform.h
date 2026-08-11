/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_platform.h
 *
 * PURPOSE:
 *   Own the shared Framework AI runtime and Helix runtime used by Studio workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns product composition only; provider-neutral AI and Helix mechanics remain inside Umicom Framework.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_PLATFORM_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_PLATFORM_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioAiPlatform UmiStudioAiPlatform;

UmiStatus umi_studio_ai_platform_create(UmiStudioAiPlatform **out_platform);
void umi_studio_ai_platform_destroy(UmiStudioAiPlatform *platform);
UmiAiRuntime *umi_studio_ai_platform_runtime(UmiStudioAiPlatform *platform);
UmiHelixRuntime *umi_studio_ai_platform_helix(UmiStudioAiPlatform *platform);
const char *umi_studio_ai_platform_default_provider(
    const UmiStudioAiPlatform *platform);

#ifdef __cplusplus
}
#endif

#endif
