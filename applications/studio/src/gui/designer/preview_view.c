/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/gui/designer/preview_view.c
 *
 * PURPOSE:
 *   Display deterministic Framework preview output in GTK4 so the UI reuses the same validation path as headless tests.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */
#include "preview_view.h"

GtkWidget *umi_studio_designer_preview_view_new(UmiStudioDesigner *designer)
{
    GtkWidget *scroll=gtk_scrolled_window_new();GtkWidget *text=gtk_text_view_new();GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));UmiDeclDiagnosticList diagnostics;char preview[16384];
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text),FALSE);gtk_text_view_set_monospace(GTK_TEXT_VIEW(text),TRUE);
    if(designer!=NULL&&umi_studio_designer_preview(designer,preview,sizeof(preview),&diagnostics)==UMI_STATUS_OK)gtk_text_buffer_set_text(buffer,preview,-1);else gtk_text_buffer_set_text(buffer,"Preview unavailable",-1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),text);return scroll;
}
