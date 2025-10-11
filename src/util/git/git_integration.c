/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/platform/win_launcher.c
 * PURPOSE: Implementation of Windows GUI entry/thunk and MSYS-aware spawn
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>              /* g_autofree, g_strfreev, g_getenv, etc.      */
#include <gio/gio.h>           /* GSubprocess / GSubprocessLauncher           */
#include <msys_env.h>          /* umi_msys_path_hint()                        */

#ifdef _WIN32                   /* The GUI entry + argv conversion is Windows-only */
  #include <windows.h>         /* HINSTANCE, PWSTR, WideCharToMultiByte       */
  #include <shellapi.h>        /* CommandLineToArgvW                          */
#endif

/*-----------------------------------------------------------------------------
 * INTERNAL: utf16_to_utf8
 *
 * PURPOSE:
 *   Convert a Windows UTF-16 string (wide char) to a newly-allocated UTF-8
 *   C string. This is used when we build a UTF-8 argv for main().
 *
 * RETURNS:
 *   Newly allocated UTF-8 string (g_free when done), or NULL on failure.
 *---------------------------------------------------------------------------*/
#ifdef _WIN32
static char *utf16_to_utf8(const wchar_t *w)
{
    if (!w) return NULL;                                                   /* Guard: null input -> null output */

    /* Query required size (in bytes) for the UTF-8 buffer, not including the
       terminating NUL (pass 0 length to get the size) */
    int needed = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
    if (needed <= 0) return NULL;                                          /* Conversion size query failed */

    /* Allocate output buffer using GLib allocator so callers can g_free() */
    char *out = g_malloc((gsize)needed);
    if (!out) return NULL;                                                 /* Allocation failed (unlikely) */

    /* Perform the actual conversion including the trailing NUL */
    int written = WideCharToMultiByte(CP_UTF8, 0, w, -1, out, needed, NULL, NULL);
    if (written <= 0) {                                                    /* Conversion failed */
        g_free(out);
        return NULL;
    }

    return out;                                                            /* Caller owns */
}

/*-----------------------------------------------------------------------------
 * INTERNAL: argv_from_windows
 *
 * PURPOSE:
 *   Build a UTF-8 argv vector from the wide-character command line Windows
 *   provides to GUI subsystem programs (wWinMain). This allows us to call
 *   the regular main(int,char**) with UTF-8 arguments, consistent across OSes.
 *
 * OUT:
 *   argc_out - set to argument count on success.
 *
 * RETURNS:
 *   NULL-terminated char** vector on success (free with g_strfreev),
 *   or NULL on failure.
 *---------------------------------------------------------------------------*/
static char **argv_from_windows(int *argc_out)
{
    int argcW = 0;                                                         /* Will receive wide-arg count  */
    LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argcW);         /* Parse raw command line       */
    if (!argvW || argcW < 0) return NULL;                                  /* Parse failed -> no argv      */

    /* Allocate UTF-8 argv array: +1 for trailing NULL sentinel */
    char **argv = g_new0(char*, (gsize)argcW + 1);

    for (int i = 0; i < argcW; ++i) {
        /* Convert each wide string to UTF-8 (empty string if conversion fails) */
        char *u8 = utf16_to_utf8(argvW[i]);
        argv[i] = u8 ? u8 : g_strdup("");                                  /* Never leave a NULL hole      */
    }
    argv[argcW] = NULL;                                                    /* NUL-terminate the vector     */

    LocalFree(argvW);                                                      /* Free Windows buffer          */
    if (argc_out) *argc_out = argcW;                                       /* Report argc to caller        */
    return argv;                                                           /* Caller frees with g_strfreev */
}
#endif /* _WIN32 */

/*-----------------------------------------------------------------------------
 * umi_win_spawn_with_msys_env
 *
 * See header for full contract. Implementation notes:
 *   - On Windows, we ask msys_env for a PATH prefix and prepend it (if present).
 *   - On non-Windows, we just honor 'cwd' and spawn with default environment.
 *   - We don’t mutate the parent process environment—only the child’s.
 *---------------------------------------------------------------------------*/
GSubprocess *umi_win_spawn_with_msys_env(const char *cwd,
                                         char * const *argv,
                                         GError **err)
{
    g_return_val_if_fail(argv != NULL, NULL);                              /* Must have argv vector        */
    g_return_val_if_fail(argv[0] != NULL, NULL);                           /* Program to exec is required  */

    /* Launcher controls child environment, cwd, stdio flags, etc.
       No special flags: caller can set up pipes later if needed. */
    g_autoptr(GSubprocessLauncher) launcher =
        g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_NONE);

    if (cwd && *cwd) {                                                     /* If caller requests a cwd, set it */
        g_subprocess_launcher_set_cwd(launcher, cwd);
    }

#ifdef _WIN32
    /* Ask MSYS helper for a PATH hint (e.g., "C:\\msys64\\usr\\bin;..."). */
    g_autofree gchar *hint = umi_msys_path_hint();                         /* May be NULL or empty string  */

    if (hint && *hint) {
        /* Compose PATH = <hint>;<existing PATH or empty> */
        const gchar *old_path = g_getenv("PATH");                          /* Read parent PATH, may be NULL */
        g_autofree gchar *combined =
            old_path ? g_strconcat(hint, ";", old_path, NULL)
                     : g_strdup(hint);

        /* Apply to child environment; overwrite any inherited PATH. */
        g_subprocess_launcher_setenv(launcher, "PATH", combined, TRUE);
    }
#endif

    /* Spawn child using argv vector; GLib resolves argv[0] using PATH if needed. */
    GSubprocess *proc = g_subprocess_launcher_spawnv(
        launcher,
        (const gchar * const *)argv,                                       /* Cast to GLib’s const type    */
        err
    );

    return proc;                                                           /* NULL on failure (err set)    */
}

#ifdef _WIN32
/* Forward declaration of 'main' so we can call it from the GUI entry point.
   This is legal and avoids pulling in CRT console subsystem. */
extern int main(int argc, char **argv);

/*-----------------------------------------------------------------------------
 * Windows GUI entry point: wWinMain
 *
 * PURPOSE:
 *   This thunk converts the process command line (UTF-16) to UTF-8 argv,
 *   then delegates to the normal main(). This keeps behavior across OSes
 *   consistent and avoids mojibake for non-ASCII arguments on Windows.
 *---------------------------------------------------------------------------*/
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      PWSTR     pCmdLine,
                      int       nCmdShow)
{
    /* Suppress “unused parameter” warnings: these are provided by the OS
       but not needed here because we forward to main(). */
    (void)hInstance;
    (void)hPrevInstance;
    (void)pCmdLine;
    (void)nCmdShow;

    int argc = 0;                                                          /* Will receive argc            */
    g_autofree char **argv = argv_from_windows(&argc);                     /* Build UTF-8 argv             */
    if (!argv) {
        /* If conversion fails (extremely rare), run main with no args. */
        char *fallbackv[] = { "umicom-studio-ide", NULL };
        return main(1, fallbackv);
    }

    /* Call the real entry (defined elsewhere in the project). */
    return main(argc, argv);
}
#endif /* _WIN32 */
/* --- IGNORE --- */