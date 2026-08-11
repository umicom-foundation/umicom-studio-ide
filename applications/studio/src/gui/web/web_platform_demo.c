/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/web/web_platform_demo.c
 *
 * PURPOSE:
 *   Provide a GTK4 demonstration window for the Framework web and multi-frontend platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The demo composes three thin GTK views over the same reusable Studio service used by command-line tests.
 */

#include <gtk/gtk.h>
#include "umicom/studio/web_platform.h"
#include "umicom/studio/browser.h"
#include "umicom/studio/charting.h"
#include "umicom/studio/frontend.h"
#include "web_preview.h"
#include "chart_preview.h"
#include "frontend_preview.h"
static void activate(GtkApplication *app,gpointer user_data){UmiStudioWebPlatform *p=(UmiStudioWebPlatform*)user_data;GtkWidget *window=gtk_application_window_new(app);GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,12);gtk_window_set_title(GTK_WINDOW(window),"Umicom Studio Web Platform");gtk_window_set_default_size(GTK_WINDOW(window),720,520);gtk_widget_set_margin_top(box,16);gtk_widget_set_margin_bottom(box,16);gtk_widget_set_margin_start(box,16);gtk_widget_set_margin_end(box,16);gtk_box_append(GTK_BOX(box),umi_studio_web_preview_new(p));gtk_box_append(GTK_BOX(box),umi_studio_chart_preview_new(p));gtk_box_append(GTK_BOX(box),umi_studio_frontend_preview_new(p));gtk_window_set_child(GTK_WINDOW(window),box);gtk_window_present(GTK_WINDOW(window));}
int main(int argc,char **argv){GtkApplication *app;UmiStudioWebPlatform *p=NULL;int rc;if(umi_studio_web_platform_create(&p)!=UMI_STATUS_OK)return 1;(void)umi_studio_browser_open(p,"https://umicom.foundation");(void)umi_studio_charting_add_close(p,1,100.0);(void)umi_studio_charting_add_close(p,2,101.0);(void)umi_studio_frontend_reset(p,UMI_FRONTEND_KIND_WEB);app=gtk_application_new("org.umicom.studio.web-platform-demo",G_APPLICATION_DEFAULT_FLAGS);g_signal_connect(app,"activate",G_CALLBACK(activate),p);rc=g_application_run(G_APPLICATION(app),argc,argv);g_object_unref(app);umi_studio_web_platform_destroy(p);return rc;}
