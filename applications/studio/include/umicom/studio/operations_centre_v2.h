/* Umicom Studio IDE | Operations Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_OPERATIONS_CENTRE_V2_H
#define UMICOM_STUDIO_OPERATIONS_CENTRE_V2_H
#include "umicom/studio/services.h"
#include "umicom/observability/observability_admin.h"
typedef struct UmiStudioOperationsCentreV2 UmiStudioOperationsCentreV2;
typedef struct UmiStudioOperationsCentreSnapshotV2 {
    UmiOperationsCentreSnapshot operations;
    char active_view[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t revision;
} UmiStudioOperationsCentreSnapshotV2;
UmiStatus umi_studio_operations_centre_v2_create(UmiStudioServices *services,UmiStudioOperationsCentreV2 **out_centre);
void umi_studio_operations_centre_v2_destroy(UmiStudioOperationsCentreV2 *centre);
UmiStatus umi_studio_operations_centre_v2_activate(UmiStudioOperationsCentreV2 *centre,const char *view_id);
UmiStatus umi_studio_operations_centre_v2_snapshot(const UmiStudioOperationsCentreV2 *centre,UmiStudioOperationsCentreSnapshotV2 *out_snapshot);
UmiOperationsCentre *umi_studio_operations_centre_v2_engine(UmiStudioOperationsCentreV2 *centre);
#endif
