/* Umicom Studio IDE | Native Operations Centre v2 command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/operations_commands_v2.h"
#include "umicom/studio/operations_views_v2.h"
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioOperationsCentreV2 *centre = NULL;
    UmiStudioOperationsCentreSnapshotV2 snapshot;
    UmiStudioOperationsViewV2 profiler;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_operations_centre_v2_create(umi_studio_bootstrap_services(bootstrap),&centre);
    if (status == UMI_STATUS_OK) status = umi_studio_operations_execute_v2(centre,UMI_STUDIO_OPERATIONS_COMMAND_SEED,NULL);
    if (status == UMI_STATUS_OK) status = umi_studio_operations_execute_v2(centre,UMI_STUDIO_OPERATIONS_COMMAND_OPEN_PROFILER,NULL);
    if (status == UMI_STATUS_OK) status = umi_studio_operations_view_v2_resolve(centre,"profiler",&profiler);
    if (status == UMI_STATUS_OK) status = umi_studio_operations_centre_v2_snapshot(centre,&snapshot);
    if (status == UMI_STATUS_OK) (void)printf("Operations Centre: %zu resources, %zu benchmarks, %zu crashes; %s has %zu items\n",snapshot.operations.resource_samples,snapshot.operations.benchmarks,snapshot.operations.crashes,profiler.title,profiler.item_count);
    umi_studio_operations_centre_v2_destroy(centre);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}
