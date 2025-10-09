/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/win_launcher.c
 * PURPOSE: Windows entry-point wrapper that converts wide argv to UTF-8 and
 *          calls the real `main(int,char**)` without recursion.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declaration of the program's real main */
int main(int argc, char **argv);

/* Utility: convert an argv[] from wide-char to UTF-8. Returns NULL on failure. */
static char **utf8_argv_from_wargv(int wargc, wchar_t **wargv, int *out_argc) {
    if (!out_argc) return NULL;
    *out_argc = 0;
    if (wargc < 0) return NULL;

    /* allocate argv array (+1 for NULL terminator) */
    char **argv8 = (char**)calloc((size_t)wargc + 1, sizeof(char*));
    if (!argv8) return NULL;

    for (int i = 0; i < wargc; ++i) {
        wchar_t *w = wargv[i];
        if (!w) { argv8[i] = NULL; continue; }
        /* First get required size (including NUL) */
        int need = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, w, -1, NULL, 0, NULL, NULL);
        if (need <= 0) {
            /* Fallback: try without strict validation */
            need = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
            if (need <= 0) { need = 1; } /* make it an empty string on hard failure */
        }
        char *buf = (char*)malloc((size_t)need);
        if (!buf) { /* out of memory: clean up and bail */
            for (int j = 0; j < i; ++j) free(argv8[j]);
            free(argv8);
            return NULL;
        }
        int wrote = WideCharToMultiByte(CP_UTF8, 0, w, -1, buf, need, NULL, NULL);
        if (wrote <= 0) { buf[0] = '\0'; }
        argv8[i] = buf;
    }
    argv8[wargc] = NULL;
    *out_argc = wargc;
    return argv8;
}

static void free_utf8_argv(char **argv8, int argc8) {
    if (!argv8) return;
    for (int i = 0; i < argc8; ++i) free(argv8[i]);
    free(argv8);
}

/* Common runner used by both wmain (console) and wWinMain (GUI). */
static int run_main_with_utf8_argv_from_w(int wargc, wchar_t **wargv) {
    int argc8 = 0;
    char **argv8 = utf8_argv_from_wargv(wargc, wargv, &argc8);
    if (!argv8) return EXIT_FAILURE;
    int rc = main(argc8, argv8);
    free_utf8_argv(argv8, argc8);
    return rc;
}

/* Console subsystem (preferred with -municode): entry is wmain */
int wmain(int argc, wchar_t **wargv) {
    return run_main_with_utf8_argv_from_w(argc, wargv);
}

/* GUI subsystem: entry is wWinMain; we synthesize argv from the command line */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow) {
    (void)hInst; (void)hPrev; (void)pCmdLine; (void)nCmdShow;
    int wargc = 0;
    wchar_t **wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    if (!wargv) {
        /* Extreme fallback: create a single-arg argv using module path */
        wchar_t modulePath[MAX_PATH];
        GetModuleFileNameW(NULL, modulePath, MAX_PATH);
        wchar_t *tmpv[2] = { modulePath, NULL };
        return run_main_with_utf8_argv_from_w(1, tmpv);
    }
    int rc = run_main_with_utf8_argv_from_w(wargc, wargv);
    LocalFree(wargv);
    return rc;
}
#endif /* _WIN32 */
