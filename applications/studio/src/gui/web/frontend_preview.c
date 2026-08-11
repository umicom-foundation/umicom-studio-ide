/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/frontend_preview.c
 *
 * PURPOSE:
 *   Render semantic frontend-plan information in GTK4.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This view proves that GTK4 can inspect the same plan used by generated web frontends.
 */

#include "frontend_preview.h"
#include <stdio.h>
GtkWidget *umi_studio_frontend_preview_new(UmiStudioWebPlatform *platform){UmiFrontendPlan *plan;char text[256];if(platform==NULL)return gtk_label_new("Frontend unavailable");plan=umi_studio_web_platform_frontend(platform);(void)snprintf(text,sizeof(text),"Frontend: %s\nPages: %zu\nRoutes: %zu\nAssets: %zu",umi_frontend_kind_text(plan->kind),plan->page_count,plan->route_count,plan->asset_count);return gtk_label_new(text);}
