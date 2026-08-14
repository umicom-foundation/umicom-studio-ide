/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_centre.h
 * PURPOSE: Own Studio's Framework-backed Helix Agent Runtime v2 composition.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: The centre owns product policy and adapters.  All operation,
 * approval, retry and evidence rules stay in reusable Framework services. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_CENTRE_H
#define UMICOM_STUDIO_HELIX_AGENT_CENTRE_H

#include "umicom/studio/helix_agent_adapters.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioHelixAgentCentreConfig {
    uint32_t maximum_attempts;
    double minimum_fitness;
    int require_human_approval;
    int allow_filesystem;
    int allow_build;
    int allow_test;
    int allow_review;
    int allow_source_control;
} UmiStudioHelixAgentCentreConfig;

typedef struct UmiStudioHelixAgentCentre UmiStudioHelixAgentCentre;

UmiStudioHelixAgentCentreConfig umi_studio_helix_agent_centre_config_default(void);
UmiStatus umi_studio_helix_agent_centre_create(
    const UmiStudioHelixAgentCentreConfig *config,
    UmiStudioHelixAgentCentre **out_centre);
void umi_studio_helix_agent_centre_destroy(UmiStudioHelixAgentCentre *centre);
UmiHelixOrchestrator *umi_studio_helix_agent_centre_runtime(
    UmiStudioHelixAgentCentre *centre);
const UmiStudioHelixAgentAdapterState *umi_studio_helix_agent_centre_adapter_state(
    const UmiStudioHelixAgentCentre *centre);

#ifdef __cplusplus
}
#endif

#endif
