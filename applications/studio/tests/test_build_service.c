/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_build_service.c
 *
 * PURPOSE:
 *   Verify Studio build-profile ownership, snapshots and retained build history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/build.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBuildService *service = NULL;
    UmiStudioBuildSnapshot snapshot;
    UmiClock clock = umi_clock_system();
    assert(umi_studio_build_service_create(".", &clock, &service) == UMI_STATUS_OK);
    assert(service != NULL);
    assert(umi_studio_build_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.profile_id, "studio.development") == 0);
    assert(strcmp(snapshot.source_root, ".") == 0);
    assert(snapshot.next_operation_id == 1U);
    assert(umi_studio_build_service_history(service) != NULL);
    assert(umi_studio_build_service_profile(service) != NULL);
    assert(umi_studio_build_service_workspace(service) != NULL);
    assert(umi_studio_build_service_prepare_default_graph(service, 1) ==
           UMI_STATUS_OK);
    assert(umi_studio_build_service_snapshot(service, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.graph.node_count == 4U);
    assert(snapshot.graph.dependency_count == 3U);
    assert(snapshot.graph.ready_count == 1U);
    assert(snapshot.workspace.graph.node_count == snapshot.graph.node_count);
    assert(snapshot.workspace.visible_node_count == 4U);
    assert(snapshot.workspace.has_selected_node);
    assert(strcmp(snapshot.workspace.selected_node_id, "configure") == 0);
    assert(snapshot.workspace.can_execute_next);
    assert(snapshot.workspace.can_execute_all);
    assert(umi_studio_build_service_artifacts(service) != NULL);
    umi_studio_build_service_destroy(service);
    umi_clock_dispose(&clock);
    return 0;
}
