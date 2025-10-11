/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/crash_guard.h
 * PURPOSE: GLib log handler routing to stderr without abort
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CRASH_GUARD_H
#define UMICOM_CRASH_GUARD_H

#include <glib.h>

/* Install a log writer function that demotes fatal logs and prints to stderr.
 * Use only during development to avoid hard-abort on g_warning/g_critical. */
void umi_crash_guard_install(void);

#endif /* UMICOM_CRASH_GUARD_H */
