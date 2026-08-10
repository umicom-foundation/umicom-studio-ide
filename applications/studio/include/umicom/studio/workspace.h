/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workspace.h
 *
 * PURPOSE:
 *   Define Studio workspace use cases over the authoritative Framework
 *   workspace graph, file index and watcher services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_WORKSPACE_H
#define UMICOM_STUDIO_WORKSPACE_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioWorkspaceSnapshot {
    UmiWorkspaceGraphSnapshot graph;
    UmiFileIndexStats files;
    UmiWatcherStats watcher;
} UmiStudioWorkspaceSnapshot;

UmiStatus umi_studio_workspace_open(UmiStudioServices *services,
                                    const char *root,
                                    int trusted,
                                    int start_watcher);
UmiStatus umi_studio_workspace_close(UmiStudioServices *services);
UmiStatus umi_studio_workspace_refresh(UmiStudioServices *services);
UmiStatus umi_studio_workspace_set_trusted(UmiStudioServices *services,
                                           int trusted);
UmiStatus umi_studio_workspace_snapshot(
    UmiStudioServices *services,
    UmiStudioWorkspaceSnapshot *out_snapshot
);
UmiStatus umi_studio_workspace_project_at(
    UmiStudioServices *services,
    size_t index,
    UmiWorkspaceProjectSnapshot *out_project
);

#ifdef __cplusplus
}
#endif

#endif
