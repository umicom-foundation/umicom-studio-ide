/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/win_launcher.c
 * PURPOSE: Windows launcher that collects UTF‑16 argv and calls run_main()
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <windows.h>
#include <shellapi.h>    /* CommandLineToArgvW */
#include <stdlib.h>
#include <string.h>

/* run_main is implemented in main.c */
extern int run_main(int argc, char **argv);

/* Convert wide argv to UTF‑8 argv. Returns heap-allocated array + strings. */
static int utf16_argv_to_utf8(int wargc, wchar_t **wargv, char ***out_argv){
    if(!out_argv) return -1;
    *out_argv = NULL;
    if(wargc <= 0 || !wargv){
        *out_argv = (char**)calloc(1, sizeof(char*));
        return 0;
    }
    char **argv8 = (char**)calloc((size_t)wargc + 1, sizeof(char*));
    if(!argv8) return -1;

    for(int i=0;i<wargc;i++){
        wchar_t *w = wargv[i];
        int need = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
        if(need <= 0){
            /* Fallback to empty string if conversion fails for this arg */
            argv8[i] = _strdup("");
            continue;
        }
        argv8[i] = (char*)malloc((size_t)need);
        if(!argv8[i]){
            /* free already allocated elements */
            for(int j=0;j<i;j++) free(argv8[j]);
            free(argv8);
            return -1;
        }
        WideCharToMultiByte(CP_UTF8, 0, w, -1, argv8[i], need, NULL, NULL);
    }
    argv8[wargc] = NULL;
    *out_argv = argv8;
    return wargc;
}

static void free_utf8_argv(int argc, char **argv){
    (void)argc;
    if(!argv) return;
    for(char **p=argv; *p; ++p) free(*p);
    free(argv);
}

/* Shared runner used by both console and GUI entry points. */
static int run_main_with_utf8_argv(void){
    int argcW = 0;
    wchar_t **argvW = CommandLineToArgvW(GetCommandLineW(), &argcW);
    if(!argvW){
        /* No args; invoke run_main with empty argv */
        char **argv8 = (char**)calloc(1, sizeof(char*));
        int rc = run_main(0, argv8);
        free(argv8);
        return rc;
    }
    char **argv8 = NULL;
    int argc8 = utf16_argv_to_utf8(argcW, argvW, &argv8);
    int rc = run_main(argc8, argv8);
    free_utf8_argv(argc8, argv8);
    LocalFree(argvW);
    return rc;
}

/* Console entry (Debug) */
#if defined(_CONSOLE) || !defined(NDEBUG)
int wmain(void){
    return run_main_with_utf8_argv();
}
#else
/* GUI entry (Release) */
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmdLine, int nShowCmd){
    (void)hInst; (void)hPrev; (void)lpCmdLine; (void)nShowCmd;
    return run_main_with_utf8_argv();
}
#endif
