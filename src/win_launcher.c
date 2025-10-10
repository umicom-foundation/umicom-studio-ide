/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/win_launcher.c
 * PURPOSE: Windows entry-point shim. Converts UTF-16 argv to UTF-8 and
 *          forwards to the regular 'main(int,char**)' without recursion.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>

/* We call the app's real main(). Do NOT define another main() here. */
extern int main(int argc, char **argv);

/* Convert wide-char argv[] to freshly-allocated UTF-8 argv[] */
static int wide_argv_to_utf8(int wargc, wchar_t **wargv, char ***out_argv) {
    int i, argc = wargc;
    char **argv = (char**)calloc((size_t)argc + 1, sizeof(char*));
    if (!argv) return -1;

    for (i = 0; i < argc; ++i) {
        int need = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
        if (need <= 0) { argc = i; break; }
        argv[i] = (char*)malloc((size_t)need);
        if (!argv[i]) { argc = i; break; }
        (void)WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], need, NULL, NULL);
    }
    argv[argc] = NULL;
    *out_argv = argv;
    return argc;
}

static void free_utf8_argv(int argc, char **argv) {
    if (!argv) return;
    for (int i = 0; i < argc; ++i) free(argv[i]);
    free(argv);
}

/* Common trampoline: build UTF-8 argv and call real main(). */
static int run_main_utf8_from_wargv(int wargc, wchar_t **wargv) {
    char **argv = NULL;
    int argc = wide_argv_to_utf8(wargc, wargv, &argv);
    if (argc < 0) return -1;
    int rc = main(argc, argv);
    free_utf8_argv(argc, argv);
    return rc;
}

/* Console subsystem + -municode -> CRT looks for wmain(). */
int wmain(int argc, wchar_t **wargv) {
    return run_main_utf8_from_wargv(argc, wargv);
}

/* GUI subsystem + -municode -> CRT looks for wWinMain(). */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR pCmdLine, int nShow) {
    (void)hInst; (void)hPrev; (void)pCmdLine; (void)nShow;
    int argc = 0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int rc = run_main_utf8_from_wargv(argc, wargv ? wargv : (wchar_t**)NULL);
    if (wargv) LocalFree(wargv);
    return rc;
}
