/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/icon.h
 *
 * PURPOSE:
 *   Declare tiny, stand-alone helpers for working with a brand/logo image
 *   inside the app. We keep this header *small* and *loosely coupled* so
 *   any GUI module can include it without dragging additional dependencies.
 *
 * DESIGN GOALS:
 *   - Pure C API (works with C compilers across platforms).
 *   - No global state: all functions return regular GTK widgets or pixbufs.
 *   - No XML resources: images are embedded as XPM fallback right in C code.
 *   - Totally optional: if you don’t call these helpers, nothing is linked.
 *
 * BEGINNER NOTE:
 *   - A “header” (.h) file contains function *declarations* (the what).
 *   - The “source” (.c) file contains function *definitions* (the how).
 *   - Other files include this header to *use* the functions defined in the C
 *     file. Keeping the header tiny reduces compile times and coupling.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-14 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ICON_H          /* Include guard: avoid double inclusion.   */
#define UMICOM_UI_ICON_H

/* Include the GTK forward declarations that callers will use.                */
#include <gtk/gtk.h>               /* GtkWidget, GtkWindow, etc.               */
#include <gdk-pixbuf/gdk-pixbuf.h> /* GdkPixbuf for in-memory images.          */

G_BEGIN_DECLS                      /* C++ compatibility (harmless in C).       */

/*-----------------------------------------------------------------------------
 * umi_icon_make_logo_image
 *
 * PURPOSE:
 *   Create a GTK image widget that shows the Umicom logo (or a fallback
 *   monochrome glyph) at approximately `px` pixels on a side.
 *
 * PARAMETERS:
 *   px  - desired logical size in pixels (e.g. 16, 24, 32). GTK will scale
 *         as needed, but providing a sensible value improves clarity.
 *
 * RETURNS:
 *   GtkWidget* pointing to a GtkImage (caller packs it into layouts).
 *
 * NOTES:
 *   - Always succeeds: if high-res assets are not available, a tiny built-in
 *     XPM fallback is used so your UI *never* lacks a brand mark.
 *---------------------------------------------------------------------------*/
GtkWidget* umi_icon_make_logo_image (int px);

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_headerbar_logo
 *
 * PURPOSE:
 *   Convenience helper that places the brand image into the given window’s
 *   header bar (left side). If the window already has a custom title widget,
 *   we simply add the logo to the *start* area so nothing breaks.
 *
 * PARAMETERS:
 *   win        - the GtkWindow that owns a GtkHeaderBar (typical in GTK4).
 *   desired_px - the approximate pixel size for the logo glyph (e.g. 16, 24).
 *
 * RETURNS:
 *   void (modifies the headerbar in place).
 *
 * SAFETY:
 *   - If `win` has no visible header bar, the function quietly does nothing.
 *---------------------------------------------------------------------------*/
void        umi_icon_try_apply_headerbar_logo (GtkWindow* win, int desired_px);

/*-----------------------------------------------------------------------------
 * umi_icon_pixbuf_fallback
 *
 * PURPOSE:
 *   Expose a tiny in-memory pixbuf for advanced callers who want direct
 *   access to the GdkPixbuf (e.g., to paint onto a surface). This returns
 *   a *new reference* that the caller must unref with g_object_unref().
 *
 * PARAMETERS:
 *   size_px - approximate logical size (we generate/scale to this).
 *
 * RETURNS:
 *   GdkPixbuf* (never NULL; uses built-in XPM if nothing better is found).
 *---------------------------------------------------------------------------*/
GdkPixbuf*  umi_icon_pixbuf_fallback (int size_px);

/*-----------------------------------------------------------------------------
 * umi_icon_try_apply_taskbar_icon_win32 (no-op on non-Windows)
 *
 * PURPOSE:
 *   Placeholder for a future enhancement: set the taskbar icon for the
 *   window at runtime using Win32 messages. GTK4 removed per-window icon
 *   API, so we leave this as a stub to keep the module future-proof.
 *
 * PARAMETERS:
 *   win - the window whose native handle we might tweak.
 *
 * RETURNS:
 *   void (currently does nothing on all platforms).
 *---------------------------------------------------------------------------*/
void        umi_icon_try_apply_taskbar_icon_win32 (GtkWindow* win);

G_END_DECLS                        /* End extern "C" for C++ consumers.        */

#endif /* UMICOM_UI_ICON_H */
