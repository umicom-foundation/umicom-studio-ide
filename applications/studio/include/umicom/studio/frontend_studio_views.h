/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_FRONTEND_STUDIO_VIEWS_H
#define UMICOM_STUDIO_FRONTEND_STUDIO_VIEWS_H
#include "umicom/studio/frontend_studio_centre.h"
typedef enum UmiStudioFrontendViewKind { UMI_STUDIO_FRONTEND_VIEW_OVERVIEW=1, UMI_STUDIO_FRONTEND_VIEW_FILES=2, UMI_STUDIO_FRONTEND_VIEW_PREVIEW=3, UMI_STUDIO_FRONTEND_VIEW_DOM=4, UMI_STUDIO_FRONTEND_VIEW_NETWORK=5, UMI_STUDIO_FRONTEND_VIEW_CONSOLE=6, UMI_STUDIO_FRONTEND_VIEW_BUILD=7, UMI_STUDIO_FRONTEND_VIEW_QUALITY=8 } UmiStudioFrontendViewKind;
typedef struct UmiStudioFrontendView { UmiStudioFrontendViewKind kind; char id[UMI_FRONTEND_DEV_ID_CAPACITY]; char title[UMI_FRONTEND_DEV_NAME_CAPACITY]; char summary[UMI_FRONTEND_DEV_TEXT_CAPACITY]; size_t row_count; uint64_t revision; } UmiStudioFrontendView;
UmiStatus umi_studio_frontend_view_build(const UmiStudioFrontendCentre *centre,UmiStudioFrontendViewKind kind,UmiStudioFrontendView *out_view);
const char *umi_studio_frontend_view_kind_id(UmiStudioFrontendViewKind kind);
#endif
