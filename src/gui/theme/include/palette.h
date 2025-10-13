/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/theme/include/palette.h
 *
 * PURPOSE:
 *   Public declarations for:
 *     (1) Command Palette (quick action launcher, optional),
 *     (2) Theme Palette   (small UI to tweak theme live, optional).
 *
 * DESIGN:
 *   - Kept *very* small and UI-focused; no deep relative includes.
 *   - Opaque handles exposed as forward-declared structs.
 *   - This header only declares API; all logic is private to .c files.
 *
 * API SUMMARY:
 *   // Command Palette (optional, can be compiled out)
 *   typedef void (*UmiCmdFn)(gpointer user);
 *   typedef struct UmiCmd { const char *name; UmiCmdFn fn; gpointer user; } UmiCmd;
 *   typedef struct UmiPalette UmiPalette;
 *   UmiPalette *umi_palette_new          (GtkWindow *parent);
 *   void        umi_palette_set_commands (UmiPalette *p, GPtrArray *cmds);
 *   void        umi_palette_open         (UmiPalette *p);
 *   void        umi_palette_free         (UmiPalette *p);
 *
 *   // Theme Palette (distinct, non-conflicting symbols)
 *   typedef struct UmiThemePalette UmiThemePalette;
 *   UmiThemePalette *umi_theme_palette_new (void);
 *   void             umi_theme_palette_open(UmiThemePalette *p, GtkWindow *parent);
 *   void             umi_theme_palette_free(UmiThemePalette *p);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_THEME_PALETTE_H
#define UMICOM_THEME_PALETTE_H

#include <gtk/gtk.h>
#include <glib.h>

/* ----------------------------- Command Palette ---------------------------- */

typedef void (*UmiCmdFn)(gpointer user);

typedef struct {
  const char *name;   /* display name; not owned here                  */
  UmiCmdFn    fn;     /* action to invoke                              */
  gpointer    user;   /* forwarded to callback                         */
} UmiCmd;

typedef struct UmiPalette UmiPalette;

UmiPalette *umi_palette_new          (GtkWindow *parent);
void        umi_palette_set_commands (UmiPalette *p, GPtrArray *cmds); /* array of UmiCmd*; not owned */
void        umi_palette_open         (UmiPalette *p);
void        umi_palette_free         (UmiPalette *p);

/* ------------------------------ Theme Palette ----------------------------- */

typedef struct UmiThemePalette UmiThemePalette;

UmiThemePalette *umi_theme_palette_new (void);
void             umi_theme_palette_open(UmiThemePalette *p, GtkWindow *parent);
void             umi_theme_palette_free(UmiThemePalette *p);

#endif /* UMICOM_THEME_PALETTE_H */
