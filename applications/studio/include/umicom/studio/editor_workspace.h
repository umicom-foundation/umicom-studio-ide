/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/editor_workspace.h
 *
 * PURPOSE:
 *   Define Studio editor-workspace composition over the reusable Framework editor session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_STUDIO_EDITOR_WORKSPACE_H
#define UMICOM_STUDIO_EDITOR_WORKSPACE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/umicom.h"
#include "umicom/studio/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioEditorWorkspace UmiStudioEditorWorkspace;
typedef struct UmiStudioEditorWorkspaceSnapshot{uint32_t struct_size;uint32_t api_version;UmiEditorSessionSnapshot editor;uint64_t revision;int available;}UmiStudioEditorWorkspaceSnapshot;
UmiStatus umi_studio_editor_workspace_create(UmiStudioServices *services,UmiStudioEditorWorkspace **out_workspace);
void umi_studio_editor_workspace_destroy(UmiStudioEditorWorkspace *workspace);
UmiStatus umi_studio_editor_workspace_snapshot(UmiStudioEditorWorkspace *workspace,UmiStudioEditorWorkspaceSnapshot *out_snapshot);
UmiEditorSession *umi_studio_editor_workspace_session(UmiStudioEditorWorkspace *workspace);
#ifdef __cplusplus
}
#endif
#endif
