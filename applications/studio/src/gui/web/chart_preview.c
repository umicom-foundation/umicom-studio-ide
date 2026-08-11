/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/chart_preview.c
 *
 * PURPOSE:
 *   Render a compact GTK4 chart-model summary for the reference workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Only presentation logic is GTK-specific; series and indicators stay in Framework.
 */

#include "chart_preview.h"
#include <stdio.h>
GtkWidget *umi_studio_chart_preview_new(UmiStudioWebPlatform *platform){UmiChartModel *model;UmiChartSnapshot snap;char text[256];if(platform==NULL)return gtk_label_new("Chart unavailable");model=umi_studio_web_platform_chart(platform);if(umi_chart_snapshot(model,&snap)!=UMI_STATUS_OK)return gtk_label_new("Chart snapshot failed");(void)snprintf(text,sizeof(text),"Chart: %s\nSeries: %zu\nPoints: %zu",snap.title,snap.series_count,snap.point_count);return gtk_label_new(text);}
