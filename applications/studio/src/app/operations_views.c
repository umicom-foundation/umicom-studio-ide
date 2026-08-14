/* Umicom Studio IDE | Operations views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/operations_views.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_studio_operations_view_resolve(const UmiStudioOperationsCentre *centre,const char *view_id,UmiStudioOperationsView *out_view)
{
    const UmiOperationsViewDescriptor *descriptor;
    UmiStudioOperationsCentreSnapshot snapshot;
    size_t item_count = 0U;
    if (centre == NULL || view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    descriptor = umi_operations_catalogue_find(view_id);
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;
    if (umi_studio_operations_centre_snapshot(centre,&snapshot) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    switch (descriptor->kind) {
        case UMI_OPERATIONS_VIEW_METRICS: item_count = snapshot.operations.evidence.metrics; break;
        case UMI_OPERATIONS_VIEW_TRACES: item_count = snapshot.operations.evidence.traces; break;
        case UMI_OPERATIONS_VIEW_PROFILER: item_count = snapshot.operations.evidence.profile_samples + snapshot.operations.resource_samples; break;
        case UMI_OPERATIONS_VIEW_BENCHMARKS: item_count = snapshot.operations.benchmarks; break;
        case UMI_OPERATIONS_VIEW_CRASHES: item_count = snapshot.operations.crashes; break;
        case UMI_OPERATIONS_VIEW_HEALTH: item_count = snapshot.operations.health.components + snapshot.operations.health.readiness_checks; break;
        case UMI_OPERATIONS_VIEW_RESILIENCE: item_count = snapshot.operations.resilience.supervised_components; break;
        case UMI_OPERATIONS_VIEW_EVENTS: item_count = snapshot.operations.evidence.events; break;
        case UMI_OPERATIONS_VIEW_AUDIT: item_count = snapshot.operations.evidence.audit_records; break;
        case UMI_OPERATIONS_VIEW_OVERVIEW: item_count = snapshot.operations.resource_samples + snapshot.operations.benchmarks + snapshot.operations.crashes; break;
        default: return UMI_STATUS_NOT_IMPLEMENTED;
    }
    (void)memset(out_view,0,sizeof(*out_view));
    (void)snprintf(out_view->view_id,sizeof(out_view->view_id),"%s",descriptor->id);
    (void)snprintf(out_view->title,sizeof(out_view->title),"%s",descriptor->title);
    (void)snprintf(out_view->summary,sizeof(out_view->summary),"%s",descriptor->description);
    out_view->item_count = item_count;
    out_view->available = true;
    return UMI_STATUS_OK;
}
