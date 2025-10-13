/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/panes/problems/include/problem_list.h
 *
 * PURPOSE:
 *   In-memory list of parsed diagnostics (“Problems” pane model).
 *
 * DESIGN:
 *   - Pure model types; no GTK/UI dependencies.
 *   - Uses UmiDiagSeverity from umi_diagnostics.h (no local enum!).
 *   - Parser(s) append problems; views subscribe to changes.
 *
 * API (minimal and stable across modules):
 *   typedef struct UmiProblem      UmiProblem;
 *   typedef struct UmiProblemList  UmiProblemList;
 *
 *   UmiProblemList* umi_problem_list_new(void);
 *   void            umi_problem_list_free(UmiProblemList *pl);
 *   void            umi_problem_list_clear(UmiProblemList *pl);
 *
 *   /* Parse one compiler/build line; append to list if it matches.
 *    * Returns true if a problem was recognized and appended. * /
 *   bool            umi_problem_parse_any(UmiProblemList *pl, const char *line_utf8);
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_PROBLEM_LIST_H
#define UMI_PROBLEM_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "umi_diagnostics.h"  /* <- canonical severity, do NOT redeclare */
#include <stddef.h>

/* One diagnostic entry recognized by a parser. */
typedef struct UmiProblem {
    UmiDiagSeverity severity; /* error / warning / note                        */
    const char     *file;     /* may be NULL if unknown                        */
    int             line;     /* 1-based, or 0 if unknown                      */
    int             col;      /* 1-based, or 0 if unknown                      */
    const char     *message;  /* human-readable                                */
} UmiProblem;

/* Opaque list type. */
typedef struct UmiProblemList UmiProblemList;

/* Lifecycle. */
UmiProblemList* umi_problem_list_new(void);
void            umi_problem_list_free(UmiProblemList *pl);
void            umi_problem_list_clear(UmiProblemList *pl);

/* Parsing entry point — tries multiple known formats (gcc/clang/msvc/etc.). */
bool            umi_problem_parse_any(UmiProblemList *pl, const char *line_utf8);

#ifdef __cplusplus
}
#endif
#endif /* UMI_PROBLEM_LIST_H */
