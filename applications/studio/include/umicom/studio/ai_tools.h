/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_tools.h
 *
 * PURPOSE:
 *   Register and invoke Studio AI tools through Framework approval policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio tools are declared Framework tools with permissions, so an AI response cannot bypass approval by calling application functions directly.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_TOOLS_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_TOOLS_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_ai_tools_register_defaults(UmiStudioAiPlatform *platform);
UmiStatus umi_studio_ai_tools_echo(UmiStudioAiPlatform *platform,
                                   const char *arguments_json,
                                   int approved,
                                   char *output,
                                   size_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
