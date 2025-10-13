/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/panes/problems/include/problem_list.h
 *
 * PURPOSE:
 *   Public API for the Problems pane: create the list, add/clear items,
 *   query size, and get the GTK widget to pack into UI containers.
 *
 * DESIGN:
 *   - The canonical diagnostics types (UmiDiag/UmiDiagSeverity) are defined
 *     in 'umi_output_sink.h'. We DO NOT duplicate any of those here.
 *   - This header is UI-facing; it exposes a GtkWidget* accessor so the
 *     editor can pack the problems view into a Notebook or Paned.
 *   - Callbacks: you can supply an optional "activate" callback that fires
 *     when the user double-clicks a problem row.
 *
 * API:
 *   typedef struct _UmiProblemList UmiProblemList;
 *   typedef void (*UmiProblemActivateCb)(gpointer user,
 *                                        const char *file, int line, int col);
 *   UmiProblemList *problem_list_new(void);
 *   UmiProblemList *problem_list_new_with_cb(UmiProblemActivateCb cb, gpointer user);
 *   void            problem_list_free  (UmiProblemList *list);
 *   gboolean        problem_list_add   (UmiProblemList *list, const UmiDiag *diag);
 *   size_t          problem_list_clear (UmiProblemList *list);
 *   size_t          problem_list_count (const UmiProblemList *list);
 *   const void     *problem_list_model (const UmiProblemList *list);
 *   GtkWidget      *problem_list_widget(const UmiProblemList *list);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#pragma once

#include <gtk/gtk.h>           /* GtkWidget for widget accessor */
#include <glib.h>              /* gboolean, gpointer, size_t   */
#include "umi_output_sink.h"   /* UmiDiag / UmiDiagSeverity     */

/* Opaque handle; callers never see internals. */
typedef struct _UmiProblemList UmiProblemList;

/* Activation callback fires when user activates a row (e.g., double-click).
 * 'file' may be empty; 'line'/'col' are >=0 or 0 if unknown. */
typedef void (*UmiProblemActivateCb)(gpointer user,
                                     const char *file, int line, int col);

/* Constructors */
UmiProblemList *problem_list_new(void);
UmiProblemList *problem_list_new_with_cb(UmiProblemActivateCb cb, gpointer user);

/* Lifetime */
void   problem_list_free  (UmiProblemList *list);

/* Mutators */
gboolean problem_list_add   (UmiProblemList *list, const UmiDiag *diag);
size_t   problem_list_clear (UmiProblemList *list);

/* Accessors */
size_t     problem_list_count (const UmiProblemList *list);
const void*problem_list_model (const UmiProblemList *list);
GtkWidget *problem_list_widget(const UmiProblemList *list);
