/* Umicom Studio IDE Tests | Operations views and commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
#include "umicom/studio/operations_views_v2.h"
int main(void)
{
    UmiStudioOperationsCentreV2 *centre = umi_test_seeded_operations_centre_v2();
    UmiStudioOperationsViewV2 profiler;
    UmiStudioOperationsCentreSnapshotV2 snapshot;
    assert(umi_studio_operations_view_v2_resolve(centre,"profiler",&profiler) == UMI_STATUS_OK);
    assert(profiler.item_count == 38U);
    assert(umi_studio_operations_execute_v2(centre,UMI_STUDIO_OPERATIONS_COMMAND_MARK_CRASH_RECOVERED,"crash-studio-1") == UMI_STATUS_OK);
    assert(umi_studio_operations_execute_v2(centre,UMI_STUDIO_OPERATIONS_COMMAND_OPEN_PROFILER,NULL) == UMI_STATUS_OK);
    assert(umi_studio_operations_centre_v2_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(snapshot.operations.unrecovered_crashes == 0U);
    assert(strcmp(snapshot.active_view,"profiler") == 0);
    umi_studio_operations_centre_v2_destroy(centre);
    return 0;
}
