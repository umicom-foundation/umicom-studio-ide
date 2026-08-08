/*-----------------------------------------------------------------------------
 * Umicom Studio IDE                                                   [project identifier banner]
 * File: src/gui/app/include/app.h                                    [physical location within the tree]
 *
 * PURPOSE:                                                            [high-level intent for this header]
 *   Public app shell types and accessors for the top-level GTK application.  [exposes the minimal public surface]
 *   Pure C, no CSS dependencies anywhere.                                  [keep dependencies minimal and portable]
 *
 * API:                                                                [public entry points summary]
 *   GtkApplication *umi_app_new(void);                                 [create the GtkApplication (caller owns)]
 *   UmiApp        *umi_app_handle(GtkApplication *app);                [map from GtkApplication* to UmiApp*]
 *   GtkWindow     *umi_app_window(UmiApp *ua);                         [fetch main window (may be NULL before activate)]
 *   struct _UmiEditor *umi_app_editor(UmiApp *ua);                     [future editor handle accessor (may be NULL)]
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT [authorship, date, and license]
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APP_H
#define UMICOM_APP_H

/* We include GTK headers so callers have the real types; this is a tiny public
 * surface for the application shell and is intentionally minimal and stable. */
#include <gtk/gtk.h>
/* Allow C++ consumers to include this header safely */    
G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * Light app handle shared across the shell (lifetime: whole app).             [a tiny, explicit container of top-level objects]
 *---------------------------------------------------------------------------*/
typedef struct UmiApp {
  GtkApplication     *app;     /* owning GtkApplication* for the process */
  GtkWindow          *win;     /* main toplevel window (may be NULL before "activate") */
  struct _UmiEditor  *editor;  /* forward-declared editor handle (optional / may be NULL) */
} UmiApp;

/*-----------------------------------------------------------------------------
 * Public API (minimal, stable)                                                [declarations of the small, stable surface]
 *---------------------------------------------------------------------------*/

/* Create the GtkApplication (caller owns). Generally called once in main(). */
GtkApplication *umi_app_new(void);

/* Look up UmiApp by GtkApplication (NULL if unknown). */
UmiApp         *umi_app_handle(GtkApplication *app);

/* Convenience accessor for the main window. May be NULL before first show. */
GtkWindow      *umi_app_window(UmiApp *ua);

/* Future editor handle accessor (may be NULL if the editor has not been created). */
struct _UmiEditor *umi_app_editor(UmiApp *ua);

G_END_DECLS

#endif /* UMICOM_APP_H */
/*  END OF FILE */