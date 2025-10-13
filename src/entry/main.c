/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/entry/main.c   (WinMain shim for Windows GUI builds)
 *
 * PURPOSE:
 *   Provide a minimal WinMain wrapper so GUI subsystem builds link cleanly,
 *   while keeping a single canonical 'main(int,char**)' for all platforms.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
#  include <windows.h>

/* Standard C entry implemented in another TU. */
extern int main(int, char **);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;
    return main(__argc, __argv);
}
#endif
