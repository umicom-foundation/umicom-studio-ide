/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/panes/problems/include/problem_list.h
 *
 * PURPOSE:
 *   Public API for the Problems pane: storage, filtering, and rendering hooks.
 *
 * DESIGN:
 *   Depends on canonical diagnostic types from src/include/umi_output_sink.h. No local typedef duplication.
 *
 * API:
 *   problem_list_new, problem_list_free, problem_list_add, problem_list_clear, problem_list_count, problem_list_model
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#pragma once

/*  WHY INCLUDE THIS SHARED HEADER?
 *  ───────────────────────────────
 *  The entire application must agree on the definition of diagnostic severities
 *  and the diagnostic record structure. The single source of truth is the
 *  project-wide public header "umi_output_sink.h". We *do not* locally re-define
 *  enums or structs here to avoid One-Definition-Rule drift, ABI mismatches, or
 *  subtle bugs.
 */
#include "umi_output_sink.h"   /* UmiDiagSeverity, UmiDiag, UmiOutputSink */

/*  OVERVIEW
 *  ────────────────────────────────────────────────────────────────────────────
 *  The Problems pane exposes a small, opaque handle that wraps the list/model
 *  used by the UI. This header provides a narrow API so other modules (router,
 *  build system) can push normalized diagnostics without taking a dependency on
 *  any UI widget types.
 *
 *  This keeps the module *loosely coupled*: core code can add problems, and the
 *  GUI layer decides how to render them.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*  OPAQUE HANDLE
 *  ────────────────────────────────────────────────────────────────────────────
 *  We hide the internal representation to keep callers from reaching into the
 *  model. This eliminates cross-module header bleed and allows us to change the
 *  internals without breaking other modules.
 */
typedef struct _UmiProblemList UmiProblemList;

/*  CONSTRUCTOR / DESTRUCTOR
 *  ────────────────────────────────────────────────────────────────────────────
 *  The list owns its storage for problems. It does not own UI widgets.
 */
UmiProblemList *problem_list_new   (void);
/*  Frees list storage; does *not* touch UI widgets owned elsewhere. */
void            problem_list_free  (UmiProblemList *list);

/*  MUTATORS
 *  ────────────────────────────────────────────────────────────────────────────
 *  Add a normalized diagnostic. The pointer is not retained after the call;
 *  we copy the record. Returns 'false' if allocation fails.
 */
gboolean        problem_list_add   (UmiProblemList *list, const UmiDiag *diag);

/*  Clears the internal storage. Returns number of items removed. */
size_t          problem_list_clear (UmiProblemList *list);

/*  ACCESSORS
 *  ────────────────────────────────────────────────────────────────────────────
 *  Get the current size; cheap O(1) accessor.
 */
size_t          problem_list_count (const UmiProblemList *list);

/*  OPTIONAL: expose a generic list model pointer for UI binding. The
 *  underlying type is intentionally not declared here to avoid a GTK include
 *  in this public header. Treat the returned pointer as opaque in callers.
 */
const void     *problem_list_model (const UmiProblemList *list);

/*  THREADING
 *  ────────────────────────────────────────────────────────────────────────────
 *  All calls are expected on the main thread unless otherwise documented.
 *  If a producer thread wants to push diagnostics, it should enqueue to the
 *  main loop (e.g., g_idle_add_full) and call 'problem_list_add' there.
 */

#ifdef __cplusplus
} /* extern "C" */
#endif

/*  END OF FILE
 *  ────────────────────────────────────────────────────────────────────────────
 *  Implementation lives in src/panes/problems/problem_list.c (not shown here).
 */
