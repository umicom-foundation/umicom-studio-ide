/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_adapters.h
 * PURPOSE: Adapt approved Helix v2 actions to Studio-owned product boundaries.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: These flags are product policy.  Framework verifies approval;
 * Studio separately decides which side-effect categories are available. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_ADAPTERS_H
#define UMICOM_STUDIO_HELIX_AGENT_ADAPTERS_H

#include <stdint.h>
#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioHelixAgentAdapterState {
    int allow_filesystem;
    int allow_build;
    int allow_test;
    int allow_review;
    int allow_source_control;
    uint32_t execution_count;
    UmiHelixActionKind last_kind;
} UmiStudioHelixAgentAdapterState;

void umi_studio_helix_agent_adapters_init(
    UmiStudioHelixAgentAdapterState *state);
UmiStatus umi_studio_helix_agent_adapters_create(
    UmiStudioHelixAgentAdapterState *state,
    UmiHelixExecutionAdapter *out_adapter);

#ifdef __cplusplus
}
#endif

#endif
