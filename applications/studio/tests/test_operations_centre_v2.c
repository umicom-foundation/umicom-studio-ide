/* Umicom Studio IDE Tests | Operations Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_v2_fixture.h"
int main(void)
{
    UmiStudioOperationsCentreV2 *centre = umi_test_seeded_operations_centre_v2();
    UmiStudioOperationsCentreSnapshotV2 snapshot;
    assert(umi_studio_operations_centre_v2_activate(centre,"health") == UMI_STATUS_OK);
    assert(umi_studio_operations_centre_v2_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(snapshot.operations.resource_samples == 2U);
    assert(snapshot.operations.benchmarks == 2U);
    assert(snapshot.operations.crashes == 1U);
    assert(strcmp(snapshot.active_view,"health") == 0);
    umi_studio_operations_centre_v2_destroy(centre);
    return 0;
}
