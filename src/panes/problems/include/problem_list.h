/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/problem_list.h
 * PURPOSE: Diagnostics model and GTK list view; parse helpers
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROBLEM_LIST_H
#define UMICOM_PROBLEM_LIST_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  UMI_DIAG_ERROR = 0,
  UMI_DIAG_WARNING = 1,
  UMI_DIAG_NOTE = 2
} UmiDiagSeverity;

typedef struct _UmiProblemList UmiProblemList;

/* Callback fired when user activates a row (double-click/Enter). */
typedef void (*UmiProblemActivateCb)(gpointer user, const char *file, int line, int col);

/* Construction and widget access */
UmiProblemList *umi_problem_list_new(UmiProblemActivateCb on_act, gpointer user);
GtkWidget      *umi_problem_list_widget(UmiProblemList *pl);

/* Model ops */
void umi_problem_list_clear(UmiProblemList *pl);
void umi_problem_list_add(UmiProblemList *pl, const char *file, int line, int col,
                          UmiDiagSeverity sev, const char *message);

/* Parsing: return TRUE if line was recognized and added to model */
gboolean umi_problem_parse_any(UmiProblemList *pl, const char *line);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_PROBLEM_LIST_H */
