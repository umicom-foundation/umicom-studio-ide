/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/crash_guard.h
 * PURPOSE: Process-wide crash/SEH signal guard install/uninstall APIs
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CRASH_GUARD_H
#define UMICOM_CRASH_GUARD_H

/*-----------------------------------------------------------------------------
 * crash_guard_install
 *
 * PURPOSE:
 *   Install crash handlers (SEH on Windows, POSIX signals elsewhere) to
 *   produce a readable diagnostic (log, message box, etc.) instead of a
 *   silent crash. Idempotent: calling twice is harmless.
 *---------------------------------------------------------------------------*/
void crash_guard_install(void);

/*-----------------------------------------------------------------------------
 * crash_guard_uninstall
 *
 * PURPOSE:
 *   Remove previously installed crash handlers. Useful in test harnesses or
 *   for sections that want default behavior restored temporarily.
 *---------------------------------------------------------------------------*/
void crash_guard_uninstall(void);

#endif /* UMICOM_CRASH_GUARD_H */
