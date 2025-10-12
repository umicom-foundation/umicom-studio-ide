/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/entry/main.c   (WinMain shim for Windows GUI builds)
 * PURPOSE:
 *   Provide a minimal WinMain wrapper so Windows GUI subsystem builds (WIN32_EXECUTABLE)
 *   link cleanly while keeping a single canonical 'main(int,char**)' entry point.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifdef _WIN32            /* Only relevant on Windows targets */
#  include <windows.h>   /* HINSTANCE, LPSTR – WinMain signature */

/* WinMain:
 *  - Windows GUI subsystem looks for this symbol when CMake sets WIN32_EXECUTABLE.
 *  - We simply forward to the standard C 'main' so all logic stays in one place.
 *
 * PSEUDOCODE:
 *   function WinMain(...):
 *       convert Windows CRT globals (__argc/__argv) to main signature
 *       return main(__argc, __argv)
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;  /* no-op: keep unused quiet */

    /* __argc/__argv are provided by the MSVCRT/MinGW CRT; they mirror the command line.
     * By forwarding to main(), we keep one code path across platforms.
     */
    extern int main(int, char **);
    return main(__argc, __argv);
}
#endif /* _WIN32 */
/* End of src/entry/main.c */