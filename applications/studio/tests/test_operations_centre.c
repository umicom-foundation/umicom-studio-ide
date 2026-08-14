/* Umicom Studio IDE Tests | Operations Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "operations_workspace_fixture.h"
int main(void)
{
    UmiStudioOperationsCentre *centre = umi_test_seeded_operations_centre();
    UmiStudioOperationsCentreSnapshot snapshot;
    assert(umi_studio_operations_centre_activate(centre,"health") == UMI_STATUS_OK);
    assert(umi_studio_operations_centre_snapshot(centre,&snapshot) == UMI_STATUS_OK);
    assert(snapshot.operations.resource_samples == 2U);
    assert(snapshot.operations.benchmarks == 2U);
    assert(snapshot.operations.crashes == 1U);
    assert(strcmp(snapshot.active_view,"health") == 0);
    umi_studio_operations_centre_destroy(centre);
    return 0;
}
