/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/include/resource.h
 *
 * PURPOSE (What is this file?):
 *   Central header for Windows resource identifiers used by the Umicom Studio
 *   IDE. These IDs are referenced by the resource script (win/umicom.rc) and
 *   by small Win32 helpers (e.g., icon.c, splash.c) to load bundled assets.
 *
 * WHAT’S DEFINED HERE:
 *   - Icon resource IDs (big + small app icons)
 *   - PNG splash image ID (embedded as RCDATA)
 *   - Safe defaults for Visual Studio’s resource editor (APSTUDIO) and
 *     IDC_STATIC, so builds are consistent across MSVC/MinGW/Clang.
 *
 * ID RANGES (guideline):
 *   100–199 : Icons / cursors
 *   200–299 : Raw data (RCDATA) such as PNGs
 *   1000+   : Dialogs, menus, accelerators (future)
 *
 * HOW TO USE:
 *   1) Include this header from your .rc file:
 *        #include "resource.h"
 *   2) In C code that loads resources at runtime, also include it when you
 *      call MAKEINTRESOURCE/FindResource, etc. (Windows only).
 *
 * CROSS-PLATFORM NOTE:
 *   - On non-Windows platforms this header is harmless; it only defines
 *     integers. You should still guard Win32 API usage with #ifdef _WIN32.
 *
 * LICENSE:
 *   MIT (see project root). Keep this header comment intact for attribution.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-15
 *---------------------------------------------------------------------------*/

#pragma once

/* --- Stable resource IDs -------------------------------------------------- */
/* Icons (Windows will auto-select the best size from the .ico container).   */
#define IDI_APPICON         101
#define IDI_APPICON_SMALL   102

/* Raw data (RCDATA) — PNG splash/logo embedded by win/umicom.rc              */
#define IDP_SPLASH          201

/* --- Compatibility shims -------------------------------------------------- */
/* Some toolchains expect IDC_STATIC; make sure it’s defined.                 */
#ifndef IDC_STATIC
#define IDC_STATIC          (-1)
#endif

/* If you decide to add dialogs/menus later, reserve ranges here:
   #define IDD_ABOUTBOX     1001
   #define IDM_FILE_OPEN    1100
   #define IDM_FILE_EXIT    1101
*/

/* --- Visual Studio resource editor defaults (harmless elsewhere) ---------- */
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE    300
#define _APS_NEXT_COMMAND_VALUE     40000
#define _APS_NEXT_CONTROL_VALUE     1000
#define _APS_NEXT_SYMED_VALUE       1
#endif
#endif
