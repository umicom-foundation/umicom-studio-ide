/* Umicom Studio IDE Tests | Operations views and commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
#include "umicom/studio/operations_views.h"
int main(void)
{
    UmiStudioOperationsCentre *centre = umi_test_seeded_operations_centre();
    UmiStudioOperationsView profiler;
    UmiStudioOperationsCentreSnapshot snapshot;
    assert(umi_studio_operations_view_resolve(centre,"profiler",&profiler) == UMI_STATUS_OK);
    assert(profiler.item_count == 38U);
    assert(umi_studio_operations_execute(centre,UMI_STUDIO_OPERATIONS_COMMAND_MARK_CRASH_RECOVERED,"crash-studio-1") == UMI_STATUS_OK);
    assert(umi_studio_operations_execute(centre,UMI_STUDIO_OPERATIONS_COMMAND_OPEN_PROFILER,NULL) == UMI_STATUS_OK);
    assert(umi_studio_operations_centre_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(snapshot.operations.unrecovered_crashes == 0U);
    assert(strcmp(snapshot.active_view,"profiler") == 0);
    umi_studio_operations_centre_destroy(centre);
    return 0;
}
