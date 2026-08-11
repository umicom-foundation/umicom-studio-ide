/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/include/web_preview.h
 *
 * PURPOSE:
 *   Declare the GTK4 view that previews Framework web-platform status.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The GTK view receives plain Studio snapshots. It does not own the HTTP server or browser state.
 */

#ifndef UMICOM_STUDIO_GUI_WEB_PREVIEW_H
#define UMICOM_STUDIO_GUI_WEB_PREVIEW_H
#include <gtk/gtk.h>
#include "umicom/studio/web_platform.h"
GtkWidget *umi_studio_web_preview_new(UmiStudioWebPlatform *platform);
#endif
