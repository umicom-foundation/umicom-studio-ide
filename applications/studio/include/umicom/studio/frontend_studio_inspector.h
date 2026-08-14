/* Umicom Studio IDE | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_FRONTEND_STUDIO_INSPECTOR_H
#define UMICOM_STUDIO_FRONTEND_STUDIO_INSPECTOR_H
#include "umicom/studio/web_platform.h"
UmiStatus umi_studio_frontend_inspector_capture_node(UmiStudioWebPlatform *platform,const UmiFrontendDomNode *node);
UmiStatus umi_studio_frontend_inspector_select_node(UmiStudioWebPlatform *platform,const char *node_id,UmiFrontendDomNode *out_node);
UmiStatus umi_studio_frontend_inspector_record_network(UmiStudioWebPlatform *platform,const UmiFrontendNetworkEntry *entry);
UmiStatus umi_studio_frontend_inspector_record_console(UmiStudioWebPlatform *platform,const UmiFrontendConsoleEntry *entry);
void umi_studio_frontend_inspector_clear(UmiStudioWebPlatform *platform);
#endif
