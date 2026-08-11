/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/frontend.c
 *
 * PURPOSE:
 *   Build Studio's semantic frontend plan and generate browser artifacts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The frontend plan describes pages and routes. It does not make React, gtk-js or a browser the owner of Studio state.
 */

#include "umicom/studio/frontend.h"
UmiStatus umi_studio_frontend_reset(UmiStudioWebPlatform *p,UmiFrontendKind kind){UmiFrontendPlan *plan;UmiFrontendPage page;UmiFrontendRoute route;UmiFrontendAsset asset;UmiStatus s;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;plan=umi_studio_web_platform_frontend(p);s=umi_frontend_plan_init(plan,"org.umicom.studio",kind);if(s==UMI_STATUS_OK)s=umi_frontend_page_init(&page,"workbench","Umicom Studio IDE","studio.workbench");if(s==UMI_STATUS_OK)s=umi_frontend_plan_add_page(plan,&page);if(s==UMI_STATUS_OK)s=umi_frontend_route_init(&route,"/","workbench");if(s==UMI_STATUS_OK)s=umi_frontend_plan_add_route(plan,&route);if(s==UMI_STATUS_OK)s=umi_frontend_asset_init(&asset,"theme","assets/umicom.css","text/css");if(s==UMI_STATUS_OK)s=umi_frontend_plan_add_asset(plan,&asset);return s;}
UmiStatus umi_studio_frontend_generate_html(UmiStudioWebPlatform *p,char *out,size_t cap){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_frontend_generate_html(umi_studio_web_platform_frontend(p),out,cap);}
UmiStatus umi_studio_frontend_generate_manifest(UmiStudioWebPlatform *p,char *out,size_t cap){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_frontend_generate_manifest(umi_studio_web_platform_frontend(p),out,cap);}
