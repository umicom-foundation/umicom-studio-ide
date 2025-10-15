/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/icon.h
 *
 * PURPOSE (What is this header for?):
 *   This is the **public interface** for the tiny “icon” module.  The module
 *   provides small branding helpers:
 *     - Access to an embedded PNG logo (always available — no file I/O).
 *     - A function to decode that PNG into a GTK/GDK texture (GdkTexture).
 *     - A convenience constructor that returns a ready-to-use GtkPicture.
 *     - Optional UI helpers (e.g., putting a small logo in a header bar).
 *
 *   The design is **loosely coupled**:
 *     - Callers include *only this header* (`#include "icon.h"`).
 *     - No other module paths appear in source (CMake supplies include dirs).
 *     - All types used here are from GTK (which the app already depends on).
 *
 * WHY KEEP AN EMBEDDED IMAGE?
 *   - So the app can show something branded even when asset files are missing.
 *   - Great for first-time contributors who clone and hit “build + run.”
 *   - You can swap the embedded bytes later without changing callers.
 *
 * HOW TO USE (read this first):
 *   1) **Include it** in any C file that needs the icon utilities:
 *        #include "icon.h"             // <<-- NOTE: short include, no paths
 *
 *      (CMake sets `target_include_directories()` so `"icon.h"` resolves to
 *       `src/ui/include/icon.h`.  Never write cross-module paths like
 *       `ui/include/icon.h` here; that couples code to the directory layout.)
 *
 *   2) **Get a widget** that already shows the logo at a given size:
 *        GtkWidget *img = umi_icon_image_logo(64);     // 64 logical pixels
 *        gtk_box_append(GTK_BOX(some_container), img); // pack like any widget
 *
 *   3) **Get a texture** if you need lower-level control:
 *        GdkTexture *tex = umi_icon_get_logo_texture();   // strong ref
 *        GtkWidget  *pic = gtk_picture_new_for_paintable(GDK_PAINTABLE(tex));
 *        g_object_unref(tex);                             // drop our reference
 *
 *   4) **Windows taskbar icon** (GTK4 note):
 *      GTK4 does not have `gtk_window_set_icon()` anymore.  For a proper
 *      taskbar/exe icon, embed an `.ico` with a **resource script (.rc)** in
 *      the executable (see `icon.c` for step-by-step comments).
 *
 * API STABILITY:
 *   - Functions here are small, self-contained, and unlikely to change.
 *   - If you add new branding helpers, declare them here to keep callers tidy.
 *
 * LICENSE:
 *   MIT (see project root). Keep the head comment intact for attribution.
 *
 * CREDIT:
 *   Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_IDE_ICON_H
#define UMICOM_STUDIO_IDE_ICON_H

/*-----------------------------------------------------------------------------
 * INCLUDES (public dependencies)
 *
 * WHY: Callers need the GTK types (GtkWidget, GtkWindow, GdkTexture, etc.)
 *      in the function signatures below.  We include the canonical GTK4
 *      umbrella header so callers don’t have to include it themselves.
 *
 * COUPLING NOTE:
 *   - This is a *public* header, so we restrict includes to widely-available
 *     public SDK headers only (GTK).  We do not include any project-private
 *     headers here to avoid unnecessary coupling between modules.
 *---------------------------------------------------------------------------*/
#include <gtk/gtk.h>   /* GTK4 core types and functions used in this API      */

/*-----------------------------------------------------------------------------
 * ADDITIONAL STANDARD INCLUDE (added to expose size_t for API below)
 *
 * WHY: The prototype for umi_icon_logo_png_data uses size_t. Including
 *      <stddef.h> ensures size_t is visible to any compilation unit that
 *      includes this header—without forcing callers to add another include.
 *---------------------------------------------------------------------------*/
#include <stddef.h>    /* size_t for umi_icon_logo_png_data() out-parameter   */

/*-----------------------------------------------------------------------------
 * umi_icon_logo_png_data
 *
 * WHAT IT RETURNS:
 *   A pointer to **embedded PNG bytes** for the Umicom logo (lives in icon.c),
 *   and (via an out-parameter) the byte length of that array.  The pointer is
 *   to **read-only static data** baked into the program image; do not free it.
 *
 * WHY IT’S USEFUL:
 *   - Lets advanced callers feed the bytes into other decoders or hashing.
 *   - Keeps call sites decoupled from file-system paths.
 *
 * PARAMETERS:
 *   out_len : size_t* — [OUT] the number of bytes available at the pointer.
 *                        Must be non-NULL; we write the length into it.
 *
 * RETURNS:
 *   const unsigned char* — read-only pointer valid for the process lifetime.
 *
 * SAFETY:
 *   Returns non-NULL and sets *out_len > 0 when compiled with the built-in
 *   stub image.  If future changes make the embed optional, returning NULL
 *   would be acceptable; call sites should handle that defensively.
 *---------------------------------------------------------------------------*/
const unsigned char* umi_icon_logo_png_data(size_t *out_len);

/*-----------------------------------------------------------------------------
 * umi_icon_get_logo_texture
 *
 * PURPOSE:
 *   Decode the embedded PNG bytes into a **GdkTexture** that GTK widgets can
 *   display (e.g., via GtkPicture).  Textures are reference-counted objects.
 *
 * RETURNS:
 *   GdkTexture* — a **new strong reference**; the caller must `g_object_unref()`.
 *
 * ERROR HANDLING:
 *   - On decode failure (unlikely for the built-in image) returns NULL.
 *
 * EXAMPLE:
 *   GdkTexture *tex = umi_icon_get_logo_texture();
 *   if (tex) {
 *     GtkWidget *pic = gtk_picture_new_for_paintable(GDK_PAINTABLE(tex));
 *     // ... pack `pic` into your layout ...
 *     g_object_unref(tex);
 *   }
 *---------------------------------------------------------------------------*/
GdkTexture* umi_icon_get_logo_texture(void);

/*-----------------------------------------------------------------------------
 * umi_icon_image_logo
 *
 * PURPOSE:
 *   Convenience helper that **builds a widget** (GtkPicture) already set up
 *   to display the logo at a requested logical size (pixels).  The widget
 *   takes its own internal reference to the underlying paintable, so you
 *   don’t need to manage the texture lifetime explicitly.
 *
 * PARAMETERS:
 *   size_px : int — desired logical size (e.g., 16, 32, 64, 128).
 *                    Values <= 0 fall back to a sensible default (64).
 *
 * RETURNS:
 *   GtkWidget* — newly created GtkPicture ready to be added to containers.
 *
 * NOTES:
 *   - GTK will handle HiDPI automatically (no manual scaling math required).
 *---------------------------------------------------------------------------*/
GtkWidget* umi_icon_image_logo(int size_px);

/*-----------------------------------------------------------------------------
 * umi_icon_apply_to_window
 *
 * PURPOSE (historical / stub in GTK4):
 *   In GTK3 you could set a per-window icon.  GTK4 removed that API.  We keep
 *   a **no-op** function for API continuity, and to teach that Windows taskbar
 *   icons should come from the **EXE resources** (.rc + .ico).
 *
 * PARAMETERS:
 *   win : GtkWindow* — ignored in GTK4 builds (safe to pass NULL).
 *
 * RETURNS:
 *   void — does nothing on GTK4; future platform-specific hooks may be added.
 *---------------------------------------------------------------------------*/
void umi_icon_apply_to_window(GtkWindow *win);

/*-----------------------------------------------------------------------------
 * OPTIONAL HELPERS (already implemented in icon.c; exposed for convenience)
 *
 * WHY EXPOSE THESE:
 *   Your current `icon.c` already contains helpers such as:
 *     - `umi_icon_try_apply_headerbar_logo()`: add a small logo to a window’s
 *       GtkHeaderBar if present (purely cosmetic, safe if headerbar missing).
 *     - `umi_icon_try_apply_taskbar_icon_win32()`: placeholder for future
 *       Win32 taskbar customization (currently a no-op to keep things simple).
 *   Declaring them here avoids ad-hoc externs elsewhere and keeps usage tidy.
 *---------------------------------------------------------------------------*/

/* Add a small logo at the “start/left” of the window’s header bar if any.     */
/* `desired_px` is a hint (e.g., 16 or 24).  No-op if the window has no        */
/* header bar or uses a custom titlebar we can’t augment safely.               */
void umi_icon_try_apply_headerbar_logo(GtkWindow *win, int desired_px);

/* Future Win32 taskbar icon hook (safe no-op today).                          */
void umi_icon_try_apply_taskbar_icon_win32(GtkWindow *win);

#endif /* UMICOM_STUDIO_IDE_ICON_H */
