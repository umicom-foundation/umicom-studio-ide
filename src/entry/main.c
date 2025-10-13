/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/entry/main.c   (WinMain shim for Windows GUI builds)
 *
 * PURPOSE:
 *   Provide a minimal WinMain wrapper so Windows GUI subsystem builds
 *   (CMake WIN32_EXECUTABLE) link cleanly while keeping a single canonical
 *   'main(int,char**)' used on every platform.
 *
 * DESIGN:
 *   - Only compiled on Windows; other platforms ignore this TU.
 *   - Forwards to the real 'main' using CRT-provided __argc/__argv.
 *   - No globals, no side-effects.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#ifdef _WIN32
#  include <windows.h>   /* HINSTANCE, LPSTR, WINAPI */

/* Forward declaration of the standard C entry point implemented elsewhere. */
extern int main(int, char **);

/* Windows GUI entry: forward to standard 'main' so logic is unified. */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;  /* unused */
    return main(__argc, __argv); /* provided by the CRT (MinGW/MSVCRT) */
}
#endif /* _WIN32 */
