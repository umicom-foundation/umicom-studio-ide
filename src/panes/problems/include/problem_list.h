/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/problems/include/problem_list.h
 *
 * PURPOSE:
 *   Minimal model+view wrapper for displaying parsed diagnostics as a list.
 *   Kept UI-level but lightweight to avoid coupling parsers to widgets.
 *
 * DESIGN:
 *   - Forward-declared struct _UmiProblemList (matches other headers).
 *   - Optional row-activate callback to let editors jump-to-location.
 *   - Public API mirrors the data ops used from routers.
 *
 * API (typical):
 *   UmiProblemList *umi_problem_list_new(void);
 *   UmiProblemList *umi_problem_list_new_with_cb(UmiProblemActivateCb, gpointer);
 *   gboolean        umi_problem_list_add(UmiProblemList*, const UmiDiag*);
 *   unsigned        umi_problem_list_clear(UmiProblemList*);
 *   GtkWidget      *umi_problem_list_widget(UmiProblemList*);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef PROBLEM_LIST_H
#define PROBLEM_LIST_H

#include <glib.h>
#include <gtk/gtk.h>
#include "umi_diag_types.h"  /* UmiDiag */

#ifdef __cplusplus
extern "C" {
#endif

/* Use the same forward name other headers already used to avoid typedef clashes */
typedef struct _UmiProblemList UmiProblemList;

/* Row-activate callback: pass file/line/col so the editor can jump */
typedef void (*UmiProblemActivateCb)(gpointer user, const char *file, int line, int col);

/* Lifecycle */
UmiProblemList *umi_problem_list_new(void);
UmiProblemList *umi_problem_list_new_with_cb(UmiProblemActivateCb cb, gpointer user);
void            umi_problem_list_free(UmiProblemList *pl);

/* Model / widget accessors */
GtkWidget      *umi_problem_list_widget(UmiProblemList *pl);
const void     *umi_problem_list_model(UmiProblemList *pl); /* for future: return underlying list */

/* Data ops */
gboolean        umi_problem_list_add(UmiProblemList *pl, const UmiDiag *diag); /* appends one row */
unsigned        umi_problem_list_clear(UmiProblemList *pl);                    /* returns removed count */
unsigned        umi_problem_list_count(UmiProblemList *pl);

#ifdef __cplusplus
}
#endif
#endif /* PROBLEM_LIST_H */