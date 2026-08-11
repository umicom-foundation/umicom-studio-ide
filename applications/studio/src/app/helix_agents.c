/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agents.c
 *
 * PURPOSE:
 *   Expose read-only Studio access to registered Helix specialist agents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The returned pointers are read-only views into the shared runtime; callers do not own or modify agent descriptors.
 */

#include "umicom/studio/helix_agents.h"

size_t umi_studio_helix_agent_count(UmiStudioAiPlatform *platform)
{
    UmiHelixRuntime *runtime = umi_studio_ai_platform_helix(platform);
    return runtime != NULL ? runtime->agents.count : 0U;
}

const UmiHelixAgent *umi_studio_helix_agent_at(UmiStudioAiPlatform *platform,
                                               size_t index)
{
    UmiHelixRuntime *runtime = umi_studio_ai_platform_helix(platform);
    if (runtime == NULL || index >= runtime->agents.count) return NULL;
    return &runtime->agents.agents[index];
}
