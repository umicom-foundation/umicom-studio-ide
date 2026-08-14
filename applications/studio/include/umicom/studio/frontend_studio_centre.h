/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Web, Browser and Frontend Studio centre
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_FRONTEND_STUDIO_CENTRE_H
#define UMICOM_STUDIO_FRONTEND_STUDIO_CENTRE_H
#include "umicom/studio/frontend_studio_settings.h"
typedef struct UmiStudioFrontendCentre { UmiStudioWebPlatform *platform; char active_pane[UMI_FRONTEND_DEV_ID_CAPACITY]; UmiFrontendQualityResult latest_audit; uint64_t revision; } UmiStudioFrontendCentre;
typedef struct UmiStudioFrontendCentreSnapshot { UmiFrontendStudioSnapshot frontend; char active_pane[UMI_FRONTEND_DEV_ID_CAPACITY]; UmiFrontendQualityResult latest_audit; uint64_t revision; } UmiStudioFrontendCentreSnapshot;
UmiStatus umi_studio_frontend_centre_init(UmiStudioFrontendCentre *centre,UmiStudioWebPlatform *platform);
UmiStatus umi_studio_frontend_centre_activate(UmiStudioFrontendCentre *centre,const char *pane_id);
UmiStatus umi_studio_frontend_centre_open_document(UmiStudioFrontendCentre *centre,const UmiFrontendDocument *document);
UmiStatus umi_studio_frontend_centre_publish(UmiStudioFrontendCentre *centre,uint64_t *out_generation);
UmiStatus umi_studio_frontend_centre_audit(UmiStudioFrontendCentre *centre,const UmiFrontendQualityMetrics *metrics);
UmiStatus umi_studio_frontend_centre_snapshot(const UmiStudioFrontendCentre *centre,UmiStudioFrontendCentreSnapshot *out_snapshot);
#endif
