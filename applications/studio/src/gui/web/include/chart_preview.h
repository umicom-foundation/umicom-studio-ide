/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/include/chart_preview.h
 *
 * PURPOSE:
 *   Declare a GTK4 summary view over the toolkit-neutral Framework chart model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The first preview is intentionally textual. Later chart renderers can draw the same model without changing Studio controllers.
 */

#ifndef UMICOM_STUDIO_GUI_CHART_PREVIEW_H
#define UMICOM_STUDIO_GUI_CHART_PREVIEW_H
#include <gtk/gtk.h>
#include "umicom/studio/web_platform.h"
GtkWidget *umi_studio_chart_preview_new(UmiStudioWebPlatform *platform);
#endif
