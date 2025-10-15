/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/icons.h
 *
 * PURPOSE:
 *   Pure-C helpers to (a) load your logo as a GdkTexture or GtkImage and
 *   (b) provide a tiny built-in fallback so the app still brands itself
 *   even if external PNG/SVG files are missing.
 *
 * FUNCTIONS:
 *   GdkTexture *uside_logo_texture_from_disk_or_fallback(int prefer_px);
 *   GtkWidget  *uside_create_logo_image(int prefer_px);
 *
 * PATHS it tries (relative to the working dir/exe dir):
 *   res/umicom-256.png, res/umicom-128.png, res/umicom.svg
 *
 * NOTE:
 *   GTK4 removed per-window icon APIs. We *show* your logo in the header bar
 *   and splash. For Windows taskbar/Alt-Tab, use the optional .rc/.ico step
 *   described in CMakeLists.txt (no XML, just a resource).
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <gtk/gtk.h>

/* Returns a strong-refed texture; caller must g_object_unref() it.           */
GdkTexture *uside_logo_texture_from_disk_or_fallback(int prefer_px);

/* Convenience: returns a GtkImage already wrapping the texture.              */
GtkWidget  *uside_create_logo_image(int prefer_px);
/*---------------------------------------------------------------------------*/
/*  END OF FILE */
/*-----------------------------------------------------------------------------