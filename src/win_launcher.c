/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: win_launcher.c
 * PURPOSE: Windows entrypoints (wWinMain for GUI builds) and UTF-8 argv thunk.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <glib.h>

extern int main(int argc, char **argv);

/* Convert wide argv to UTF-8 and forward to real main() */
static int run_main_with_utf8_argv(void) {
    int argc = 0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) {
        fputs("CommandLineToArgvW failed\n", stderr);
        return -1;
    }
    char **argv = g_new0(char*, argc + 1);
    for (int i = 0; i < argc; ++i) {
        argv[i] = g_utf16_to_utf8((const gunichar2*)wargv[i], -1, NULL, NULL, NULL);
        if (!argv[i]) argv[i] = g_strdup("");
    }
    argv[argc] = NULL;
    int rc = main(argc, argv);
    for (int i = 0; i < argc; ++i) g_free(argv[i]);
    g_free(argv);
    LocalFree(wargv);
    return rc;
}

#ifdef _WIN32
#ifdef USIDE_DEV_CONSOLE
/* Console builds: let MinGW provide mainCRTStartup -> main */
#else
/* GUI builds: provide wWinMain and call our UTF-8 thunk */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow) {
    (void)hInst; (void)hPrev; (void)pCmdLine; (void)nCmdShow;
    return run_main_with_utf8_argv();
}
#endif
#endif
