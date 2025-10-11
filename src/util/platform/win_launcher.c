\
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/win_launcher.c
 * PURPOSE: Windows entrypoint shim (GUI builds). Converts UTF-16 argv to UTF-8
 *          and forwards to the real main() without recursion.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
#include <windows.h>
#include <glib.h>

/* We only provide a GUI entry when building the Windows GUI subsystem.
 * For console builds, the regular int main(int,char**) in main.c is used.
 */
#ifdef USIDE_GUI_BUILD
extern int main(int argc, char **argv);

static char **utf8_argv_from_windows(int *out_argc) {
    int argc = 0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!wargv) {
        *out_argc = 0;
        return NULL;
    }

    char **argv = (char**)g_malloc0_n((size_t)argc + 1, sizeof(char*));
    for (int i = 0; i < argc; ++i) {
        argv[i] = g_utf16_to_utf8(wargv[i], -1, NULL, NULL, NULL);
        if (!argv[i]) {
            // Fallback: empty string if conversion fails
            argv[i] = g_strdup("");
        }
    }
    argv[argc] = NULL;
    *out_argc = argc;
    LocalFree(wargv);
    return argv;
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmdLine, int nShow) {
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShow;
    int argc = 0;
    char **argv = utf8_argv_from_windows(&argc);
    int code = main(argc, argv);
    if (argv) {
        for (int i = 0; i < argc; ++i) g_free(argv[i]);
        g_free(argv);
    }
    return code;
}
#endif /* USIDE_GUI_BUILD */
#endif /* _WIN32 */
