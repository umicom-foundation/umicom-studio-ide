/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/web_preview.c
 *
 * PURPOSE:
 *   Render Framework web-platform status in a small GTK4 preview panel.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * GTK4 formatting lives here; the data comes from the toolkit-neutral UmiStudioWebPlatform snapshot.
 */

#include "web_preview.h"
#include <stdio.h>
GtkWidget *umi_studio_web_preview_new(UmiStudioWebPlatform *platform){UmiStudioWebPlatformSnapshot snap;char text[512];GtkWidget *box,*label;if(platform==NULL)return gtk_label_new("Web platform unavailable");(void)umi_studio_web_platform_snapshot(platform,&snap);(void)snprintf(text,sizeof(text),"Routes: %zu\nEndpoints: %zu\nRequests: %llu\nBrowser: %s",snap.routes,snap.endpoints,(unsigned long long)snap.requests,snap.browser_url[0]!='\0'?snap.browser_url:"not navigated");box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);label=gtk_label_new(text);gtk_label_set_xalign(GTK_LABEL(label),0.0F);gtk_box_append(GTK_BOX(box),label);return box;}
