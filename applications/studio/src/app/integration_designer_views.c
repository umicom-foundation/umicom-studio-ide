/* Umicom Studio IDE | Integration Designer views | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/integration_designer_views.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

const char *umi_studio_integration_designer_view_id(UmiStudioIntegrationDesignerViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_INTEGRATION_VIEW_OVERVIEW: return "overview";
        case UMI_STUDIO_INTEGRATION_VIEW_CONNECTIONS: return "connections";
        case UMI_STUDIO_INTEGRATION_VIEW_API_EXPLORER: return "api-explorer";
        case UMI_STUDIO_INTEGRATION_VIEW_MAPPING: return "mapping";
        case UMI_STUDIO_INTEGRATION_VIEW_WORKFLOW: return "workflow";
        case UMI_STUDIO_INTEGRATION_VIEW_EXECUTION: return "execution";
        case UMI_STUDIO_INTEGRATION_VIEW_MONITORING: return "monitoring";
        default: return "unknown";
    }
}

static const char *view_title(UmiStudioIntegrationDesignerViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_INTEGRATION_VIEW_OVERVIEW: return "Integration Fabric";
        case UMI_STUDIO_INTEGRATION_VIEW_CONNECTIONS: return "Connections";
        case UMI_STUDIO_INTEGRATION_VIEW_API_EXPLORER: return "API Explorer";
        case UMI_STUDIO_INTEGRATION_VIEW_MAPPING: return "Field Mapping";
        case UMI_STUDIO_INTEGRATION_VIEW_WORKFLOW: return "Workflow Designer";
        case UMI_STUDIO_INTEGRATION_VIEW_EXECUTION: return "Execution";
        case UMI_STUDIO_INTEGRATION_VIEW_MONITORING: return "Monitoring";
        default: return "Integration Designer";
    }
}

UmiStatus umi_studio_integration_designer_view_build(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerViewKind kind,UmiStudioIntegrationDesignerView *out_view)
{
    UmiStudioIntegrationDesignerSnapshot snapshot;
    int length;
    UmiStatus status;
    if (centre == NULL || out_view == NULL || kind < UMI_STUDIO_INTEGRATION_VIEW_OVERVIEW || kind > UMI_STUDIO_INTEGRATION_VIEW_MONITORING) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_integration_designer_snapshot(centre,&snapshot);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_view,0,sizeof(*out_view));
    out_view->kind = kind;
    status = umi_integration_designer_copy(out_view->id,sizeof(out_view->id),umi_studio_integration_designer_view_id(kind));
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_view->title,sizeof(out_view->title),view_title(kind));
    if (status != UMI_STATUS_OK) return status;
    switch (kind) {
        case UMI_STUDIO_INTEGRATION_VIEW_CONNECTIONS: out_view->item_count = snapshot.fabric.connections; break;
        case UMI_STUDIO_INTEGRATION_VIEW_API_EXPLORER: out_view->item_count = snapshot.fabric.contracts; break;
        case UMI_STUDIO_INTEGRATION_VIEW_MAPPING: out_view->item_count = snapshot.fabric.mappings; break;
        case UMI_STUDIO_INTEGRATION_VIEW_WORKFLOW: out_view->item_count = snapshot.fabric.workflows; break;
        case UMI_STUDIO_INTEGRATION_VIEW_EXECUTION:
        case UMI_STUDIO_INTEGRATION_VIEW_MONITORING: out_view->item_count = snapshot.fabric.runs.total; break;
        default: out_view->item_count = snapshot.fabric.connections + snapshot.fabric.contracts + snapshot.fabric.workflows; break;
    }
    length = snprintf(out_view->summary,sizeof(out_view->summary),"%s: %zu item(s), %zu successful run(s), revision %" PRIu64,out_view->title,out_view->item_count,snapshot.fabric.runs.succeeded,snapshot.revision);
    if (length < 0 || (size_t)length >= sizeof(out_view->summary)) return UMI_STATUS_CAPACITY_EXCEEDED;
    out_view->revision = snapshot.revision;
    return UMI_STATUS_OK;
}
