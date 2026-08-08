/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/crash_guard.c
 * PURPOSE: Minimal crash guard hook points (enable/disable and handler)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>        /* g_warning, g_critical */
#include <signal.h>      /* signal()              */

#include <crash_guard.h> /* Public API for crash guard */

/* Internal singleton flag indicating whether our handler is active. */
/* We keep it file-local to avoid exposing extra state.              */
static volatile sig_atomic_t g_umi_crash_guard_enabled = 0;

/* Internal crash handler: keep it extremely small and async-signal-safe.      */
/* NOTE: printing via GLib is not async-signal-safe; we only set a flag here.  */
static void umi_crash_signal_handler(int sig) {
    (void)sig; /* we don’t branch on the exact signal in this minimal guard */
    g_umi_crash_guard_enabled = 2; /* mark “tripped”; polled by app if desired */
    /* In a more advanced version, we could write to a pipe or abort with code. */
}

/* Enable the crash guard: installs a minimal signal handler for common faults. */
void umi_crash_guard_enable(void) {
    if (g_umi_crash_guard_enabled) return; /* already enabled or tripped */

    /* Install a very small handler for a few fatal signals.
     * IMPORTANT: keep handlers trivial to remain async-signal-safe. */
    signal(SIGSEGV, umi_crash_signal_handler);
    signal(SIGILL,  umi_crash_signal_handler);
    signal(SIGFPE,  umi_crash_signal_handler);
#ifdef SIGBUS
    signal(SIGBUS,  umi_crash_signal_handler);
#endif
    g_umi_crash_guard_enabled = 1; /* enabled */
}

/* Disable the crash guard: restore default handlers where applicable. */
void umi_crash_guard_disable(void) {
    if (!g_umi_crash_guard_enabled) return; /* not enabled */

    signal(SIGSEGV, SIG_DFL);
    signal(SIGILL,  SIG_DFL);
    signal(SIGFPE,  SIG_DFL);
#ifdef SIGBUS
    signal(SIGBUS,  SIG_DFL);
#endif
    g_umi_crash_guard_enabled = 0; /* disabled */
}
/* Check if the crash guard was tripped since last enabled/checked. */