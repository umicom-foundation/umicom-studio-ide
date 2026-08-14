/* Umicom Studio IDE | Operations Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/operations_centre_v2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiStudioOperationsCentreV2 {
    UmiStudioServices *services;
    UmiOperationsCentre operations;
    char active_view[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t revision;
};
UmiStatus umi_studio_operations_centre_v2_create(UmiStudioServices *services,UmiStudioOperationsCentreV2 **out_centre)
{
    UmiStudioOperationsCentreV2 *centre;
    if (services == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioOperationsCentreV2 *)calloc(1U,sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->services = services;
    umi_operations_centre_init(&centre->operations);
    (void)snprintf(centre->active_view,sizeof(centre->active_view),"overview");
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}
void umi_studio_operations_centre_v2_destroy(UmiStudioOperationsCentreV2 *centre) { free(centre); }
UmiStatus umi_studio_operations_centre_v2_activate(UmiStudioOperationsCentreV2 *centre,const char *view_id)
{
    int length;
    if (centre == NULL || view_id == NULL || umi_operations_catalogue_find(view_id) == NULL) return UMI_STATUS_NOT_FOUND;
    length = snprintf(centre->active_view,sizeof(centre->active_view),"%s",view_id);
    if (length < 0 || (size_t)length >= sizeof(centre->active_view)) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_studio_operations_centre_v2_snapshot(const UmiStudioOperationsCentreV2 *centre,UmiStudioOperationsCentreSnapshotV2 *out_snapshot)
{
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    umi_operations_centre_snapshot(&centre->operations,&out_snapshot->operations);
    (void)snprintf(out_snapshot->active_view,sizeof(out_snapshot->active_view),"%s",centre->active_view);
    out_snapshot->revision = centre->revision + out_snapshot->operations.revision;
    return UMI_STATUS_OK;
}
UmiOperationsCentre *umi_studio_operations_centre_v2_engine(UmiStudioOperationsCentreV2 *centre) { return centre == NULL ? NULL : &centre->operations; }
