/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/log/profiler.c
 * PURPOSE: Tiny GTimer-based profiling helpers (scoped + manual)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "profiler.h"  /* UmiProfiler API */

void
umi_profiler_start(UmiProfiler *p, const char *label)
{
    g_return_if_fail(p != NULL);                    /* Validate handle */

    if (p->timer) {
        g_timer_destroy(p->timer);                  /* Restarting: drop old timer */
        p->timer = NULL;
    }

    p->label = label ? label : "";                  /* Keep a label for convenience */
    p->timer = g_timer_new();                       /* Start a fresh timer */
    g_timer_start(p->timer);                        /* Ensure timer is running */
}

double
umi_profiler_stop_ms(UmiProfiler *p)
{
    g_return_val_if_fail(p != NULL && p->timer != NULL, 0.0); /* Must have started */
    g_timer_stop(p->timer);                        /* Stop to freeze elapsed */
    gdouble secs = g_timer_elapsed(p->timer, NULL);/* Seconds as double */
    g_timer_destroy(p->timer);                     /* Free timer */
    p->timer = NULL;                               /* Mark stopped */
    return secs * 1000.0;                          /* Convert to milliseconds */
}
/* Local helper: spawn "git <argv>" with optional @cwd and capture exit/status.
 * On success:
 *   - @out_stdout is set to newly-allocated UTF-8 text (may be empty string).
 *   - returns TRUE if process exit status == 0, else FALSE.
 * On failure to spawn/communicate, returns FALSE and sets @error.
 * If @out_stdout is NULL, stdout is read and discarded.
 *
 * Notes:
 *  - We use GSubprocessLauncher so we can set a working directory (cwd).
 *  - We pipe both STDOUT and STDERR; @error includes stderr text on failures.
 */