/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_workspace.h
 *
 * PURPOSE:
 *   Define Studio AI workspace composing AuthorEngine, retrieval, tools and Helix governance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_WORKSPACE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_WORKSPACE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioAiWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char area_id[128];
    char view_type[128];
    char title[256];
    char summary[512];
    uint64_t revision;
    size_t item_count;
    size_t provider_count;
    size_t runtime_count;
    size_t healthy_runtime_count;
    size_t context_source_count;
    size_t session_count;
    uint32_t context_limit;
    uint32_t reserved_output_tokens;
    int remote_allowed;
    int persistence_allowed;
    char active_session_id[UMI_AI_ID_CAPACITY];
    int available;
} UmiStudioAiWorkspaceSnapshot;
UmiStatus umi_studio_ai_workspace_snapshot(UmiStudioServices *services, UmiStudioAiWorkspaceSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
