/*-----------------------------------------------------------------------------
 * Umicom Studio IDE                                                             /* project identifier banner */
 * File: src/gui/app/include/app.h                                               /* physical location within the tree */
 *
 * PURPOSE:                                                                      /* high-level intent for this header */
 *   Public app shell types and accessors for the top-level GTK application.     /* exposes the minimal public surface */
 *   Pure C, no CSS dependencies anywhere.                                       /* keep dependencies minimal and portable */
 *
 * API:                                                                          /* public entry points summary */
 *   GtkApplication *umi_app_new(void);                                          /* create the GtkApplication (caller owns) */
 *   UmiApp        *umi_app_handle(GtkApplication *app);                         /* map from GtkApplication* to UmiApp* */
 *   GtkWindow     *umi_app_window(UmiApp *ua);                                   /* fetch main window (may be NULL before activate) */
 *   struct _UmiEditor *umi_app_editor(UmiApp *ua);                               /* future editor handle accessor (may be NULL) */
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT /* authorship, date, and license */
 *---------------------------------------------------------------------------*/   /* end banner header block */

#ifndef UMICOM_APP_H                                                           /* conventional include guard begin */
#define UMICOM_APP_H                                                           /* guard symbol definition */

/* Standard library / toolkit includes (kept tiny in headers) */               /* keep headers light and fast */
#include <gtk/gtk.h>                                                           /* GTK4 primary UI toolkit types */
#include <glib.h>                                                              /* GLib core utilities/macros */

/* Allow C++ consumers to include this header safely */                        /* extern "C" for C++ callers */
G_BEGIN_DECLS                                                                   /* expands to extern "C" { on C++ and nothing on C */

/* Forward declaration to avoid hard dependency on editor headers here */      /* reduces coupling and compile time */
typedef struct _UmiEditor UmiEditor;                                           /* opaque forward-declared editor type */

/*-----------------------------------------------------------------------------
 * Light app handle shared across the shell (lifetime: whole app).             /* a tiny, explicit container of top-level objects */
 *---------------------------------------------------------------------------*/ /* section header for the public struct */
typedef struct UmiApp {                                                        /* public struct so callers can inspect fields if needed */
  GtkApplication *app;   /* owning application (never NULL after umi_app_new)*/ /* back-pointer to the GtkApplication instance */
  GtkWindow      *win;   /* main toplevel window (NULL until first activate)  */ /* set in activate; may be presented multiple times */
  GtkWidget      *root;  /* root content container (central notebook/box)     */ /* main child placed into GtkWindow */
  UmiEditor      *ed;    /* editor instance handle (optional, may be NULL)    */ /* lets other modules retrieve the editor facade */
} UmiApp;                                                                       /* typedef name closes the struct */

/*-----------------------------------------------------------------------------
 * Public API (minimal, stable)                                                /* declarations of the small, stable surface */
 *---------------------------------------------------------------------------*/ /* section header for prototypes */
GtkApplication *umi_app_new(void);                                             /* create and return a new GtkApplication */
UmiApp         *umi_app_handle(GtkApplication *app);                            /* look up UmiApp by GtkApplication (NULL if unknown) */
GtkWindow      *umi_app_window(UmiApp *ua);                                     /* convenience accessor for the main window */
UmiEditor      *umi_app_editor(UmiApp *ua);                                     /* convenience accessor for editor handle (future) */

/* Close the C++ extern "C" block if it was opened */                           /* symmetry with G_BEGIN_DECLS */
G_END_DECLS                                                                      /* ends extern "C" on C++ and no-op on C */

#endif /* UMICOM_APP_H */                                                       /* conventional include guard end */
