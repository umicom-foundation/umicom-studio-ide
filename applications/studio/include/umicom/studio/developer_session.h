/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/developer_session.h
 * PURPOSE: Track the active developer context shared by Studio workbench areas.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DEVELOPER_SESSION_H
#define UMICOM_STUDIO_DEVELOPER_SESSION_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDeveloperSession UmiStudioDeveloperSession;
typedef struct UmiStudioDeveloperSessionSnapshot {uint32_t struct_size;uint32_t api_version;char project_id[128];char configuration_id[128];char launch_profile_id[128];char repository_id[128];char test_item_id[128];char active_file[512];uint64_t revision;} UmiStudioDeveloperSessionSnapshot;
UmiStatus umi_studio_developer_session_create(UmiStudioDeveloperSession **out_session);
void umi_studio_developer_session_destroy(UmiStudioDeveloperSession *session);
UmiStatus umi_studio_developer_session_set_context(UmiStudioDeveloperSession *session,const UmiStudioDeveloperSessionSnapshot *context);
UmiStatus umi_studio_developer_session_snapshot(const UmiStudioDeveloperSession *session,UmiStudioDeveloperSessionSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
