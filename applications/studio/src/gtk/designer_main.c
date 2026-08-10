/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gtk/designer_main.c
 *
 * PURPOSE:
 *   Launch the standalone GTK4 visual-designer reference window for local acceptance testing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#include <gtk/gtk.h>
#include "umicom/studio/declarative.h"
#include "umicom/studio/designer.h"
#include "designer_window.h"

typedef struct AppState{UmiStudioDeclarative *declarative;UmiStudioDesigner *designer;}AppState;
static void activate(GtkApplication *app,gpointer user_data){AppState *state=(AppState *)user_data;GtkWidget *window=umi_studio_designer_window_new(app,state->declarative,state->designer);gtk_window_present(GTK_WINDOW(window));}
int main(int argc,char **argv){GtkApplication *app;AppState state={0};int result;if(umi_studio_declarative_create(&state.declarative)!=UMI_STATUS_OK)return 1;if(umi_studio_designer_create(state.declarative,&state.designer)!=UMI_STATUS_OK){umi_studio_declarative_destroy(state.declarative);return 1;}app=gtk_application_new("org.umicom.studio.designer",G_APPLICATION_DEFAULT_FLAGS);g_signal_connect(app,"activate",G_CALLBACK(activate),&state);result=g_application_run(G_APPLICATION(app),argc,argv);g_object_unref(app);umi_studio_designer_destroy(state.designer);umi_studio_declarative_destroy(state.declarative);return result;}
