/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_settings.h
 * PURPOSE: Apply typed Helix v2 governance settings to Studio AI composition.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Settings are validated before the agent centre is created,
 * preserving a safe default even when a settings file is malformed. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_SETTINGS_H
#define UMICOM_STUDIO_HELIX_AGENT_SETTINGS_H

#include "umicom/studio/ai_platform.h"
#include "umicom/studio/settings.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_agent_settings_apply(
    const UmiSettings *settings,
    UmiStudioAiPlatformConfig *config);

#ifdef __cplusplus
}
#endif

#endif
