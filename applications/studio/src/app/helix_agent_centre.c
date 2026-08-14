/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_agent_centre.c
 * PURPOSE: Compose Framework Helix v2 orchestration with Studio adapters.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/helix_agent_centre.h"

#include <stdlib.h>

struct UmiStudioHelixAgentCentre {
    UmiHelixOrchestrator runtime;
    UmiStudioHelixAgentAdapterState adapter_state;
};

UmiStudioHelixAgentCentreConfig umi_studio_helix_agent_centre_config_default(void)
{
    UmiStudioHelixAgentCentreConfig config;
    config.maximum_attempts = 3U;
    config.minimum_fitness = 0.80;
    config.require_human_approval = 1;
    config.allow_filesystem = 1;
    config.allow_build = 1;
    config.allow_test = 1;
    config.allow_review = 1;
    config.allow_source_control = 0;
    return config;
}

UmiStatus umi_studio_helix_agent_centre_create(
    const UmiStudioHelixAgentCentreConfig *config,
    UmiStudioHelixAgentCentre **out_centre)
{
    UmiStudioHelixAgentCentre *centre;
    UmiHelixOrchestratorConfig runtime_config;
    UmiHelixExecutionAdapter adapter;
    UmiStatus status;
    if (config == NULL || out_centre == NULL || config->maximum_attempts == 0U ||
        config->minimum_fitness < 0.0 || config->minimum_fitness > 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_centre = NULL;
    centre = calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    runtime_config = umi_helix_orchestrator_config_default();
    runtime_config.maximum_attempts = config->maximum_attempts;
    runtime_config.minimum_fitness = config->minimum_fitness;
    runtime_config.require_human_approval = config->require_human_approval;
    status = umi_helix_orchestrator_init(&centre->runtime, &runtime_config);
    umi_studio_helix_agent_adapters_init(&centre->adapter_state);
    centre->adapter_state.allow_filesystem = config->allow_filesystem;
    centre->adapter_state.allow_build = config->allow_build;
    centre->adapter_state.allow_test = config->allow_test;
    centre->adapter_state.allow_review = config->allow_review;
    centre->adapter_state.allow_source_control = config->allow_source_control;
    if (status == UMI_STATUS_OK) status = umi_studio_helix_agent_adapters_create(
        &centre->adapter_state, &adapter);
    if (status == UMI_STATUS_OK) status = umi_helix_orchestrator_set_adapter(
        &centre->runtime, &adapter);
    if (status != UMI_STATUS_OK) {
        free(centre);
        return status;
    }
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_helix_agent_centre_destroy(UmiStudioHelixAgentCentre *centre)
{
    free(centre);
}

UmiHelixOrchestrator *umi_studio_helix_agent_centre_runtime(
    UmiStudioHelixAgentCentre *centre)
{
    return centre != NULL ? &centre->runtime : NULL;
}

const UmiStudioHelixAgentAdapterState *umi_studio_helix_agent_centre_adapter_state(
    const UmiStudioHelixAgentCentre *centre)
{
    return centre != NULL ? &centre->adapter_state : NULL;
}
