/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agents.h
 *
 * PURPOSE:
 *   List the specialist Helix agents registered for Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Agent inspection lets the UI show roles and permissions instead of presenting Helix as one opaque autonomous process.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_AGENTS_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_AGENTS_H

#include <stddef.h>
#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_studio_helix_agent_count(UmiStudioAiPlatform *platform);
const UmiHelixAgent *umi_studio_helix_agent_at(UmiStudioAiPlatform *platform,
                                               size_t index);

#ifdef __cplusplus
}
#endif

#endif
