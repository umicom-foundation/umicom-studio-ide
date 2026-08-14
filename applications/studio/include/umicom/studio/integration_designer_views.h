/* Umicom Studio IDE | Integration Designer views | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_INTEGRATION_DESIGNER_VIEWS_H
#define UMICOM_STUDIO_INTEGRATION_DESIGNER_VIEWS_H
#include "umicom/studio/integration_designer_centre.h"
typedef enum UmiStudioIntegrationDesignerViewKind {
    UMI_STUDIO_INTEGRATION_VIEW_OVERVIEW = 1,
    UMI_STUDIO_INTEGRATION_VIEW_CONNECTIONS,
    UMI_STUDIO_INTEGRATION_VIEW_API_EXPLORER,
    UMI_STUDIO_INTEGRATION_VIEW_MAPPING,
    UMI_STUDIO_INTEGRATION_VIEW_WORKFLOW,
    UMI_STUDIO_INTEGRATION_VIEW_EXECUTION,
    UMI_STUDIO_INTEGRATION_VIEW_MONITORING
} UmiStudioIntegrationDesignerViewKind;
typedef struct UmiStudioIntegrationDesignerView {
    UmiStudioIntegrationDesignerViewKind kind;
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char title[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    char summary[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
    size_t item_count;
    uint64_t revision;
} UmiStudioIntegrationDesignerView;
const char *umi_studio_integration_designer_view_id(UmiStudioIntegrationDesignerViewKind kind);
UmiStatus umi_studio_integration_designer_view_build(const UmiStudioIntegrationDesignerCentre *centre,UmiStudioIntegrationDesignerViewKind kind,UmiStudioIntegrationDesignerView *out_view);
#endif
