/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/include/profiler.h
 * PURPOSE: Tiny GTimer-based profiling helpers (scoped + manual)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#ifndef UMICOM_PROFILER_H
#define UMICOM_PROFILER_H

#include <glib.h>  /* GTimer */

/* Manual profile session holder. */
typedef struct {
    GTimer      *timer;   /* Owned timer */
    const char  *label;   /* Non-owned: description for logs */
} UmiProfiler;

/* Start a profile session. (idempotent if called again after stop) */
void   umi_profiler_start  (UmiProfiler *p, const char *label);

/* Stop and return elapsed milliseconds since start. */
double umi_profiler_stop_ms(UmiProfiler *p);

/* Convenience: scoped profiling macro; logs on leaving scope (via g_autoptr-like cleanup). */
#define USTUDIO_PROFILE_SCOPE(name_literal) \
    for (UmiProfiler _prof = {0}; \
         _prof.timer == NULL ? (umi_profiler_start(&_prof, (name_literal)), 1) : 0; \
         g_printerr("[PROF] %s: %.3f ms\n", (name_literal), umi_profiler_stop_ms(&_prof)))

#endif /* UMICOM_PROFILER_H */
/* End of src/util/log/include/profiler.h */
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/git/git_integration.c
 * PURPOSE: Implementation of Git helpers (child process + capture)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
 