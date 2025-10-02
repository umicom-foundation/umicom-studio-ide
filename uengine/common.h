/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: include/ueng/common.h
 * Purpose: Platform shims, portable filesystem & string utilities, process exec.
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab + contributors
 * License: MIT
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Module notes (added, non-destructive):
 * MODULE OVERVIEW
 * ----------------
 * Purpose: Cross-platform helpers shared across the engine.
 * Contents:
 *   - Tiny dynamic string list (StrList) used to accumulate filenames.
 *   - Portable filesystem probes (file_exists/dir_exists) and mkdir helpers.
 *   - Time helpers used for stamping build artifacts.
 *   - Small process runner utilities for invoking external tools (Pandoc, etc).

 * Design notes:
 *   - Keep headers minimal and C89/C17 friendly.
 *   - Callers should prefer the wrappers here instead of sprinkling #ifdefs.
 *   - Functions are intentionally small to ease portability and testing.

 * Usage example:
 *   // Get a UTC build date string
 *   char date[16]; build_date_utc(date, sizeof(date));
 *   printf("Built on %s\n", date);
 *---------------------------------------------------------------------------*/

#ifndef UENG_COMMON_H
#define UENG_COMMON_H

/* Ensure FILE and size_t are known for prototypes below */
#include <stddef.h> /* size_t */
#include <stdio.h>  /* FILE */

/* Platform differences
   --------------------
   We normalize path separators, mkdir, and 'access' across Windows/POSIX so
   the rest of the code can use PATH_SEP, make_dir(), etc. */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <io.h>
#include <windows.h>
#define PATH_SEP '\\'
#define access _access
#define ACCESS_EXISTS 0
#ifndef strdup
#define strdup _strdup
#endif
/* Small wrapper so call sites don't need #ifdefs */
static inline int make_dir(const char *p) { return _mkdir(p); }
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATH_SEP '/'
#define ACCESS_EXISTS 0
static inline int make_dir(const char *p) { return mkdir(p, 0755); }
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /*------------------------------ String list ---------------------------------*/
  /* A tiny dynamically-growing array of C strings (heap-owns its items).
     sl_init/sl_free manage memory; sl_push appends a copy of the string. */
  typedef struct
  {
    char **items;
    size_t count;
    size_t cap;
  } StrList;

  void sl_init(StrList *sl);
  void sl_free(StrList *sl);
  int sl_push(StrList *sl, const char *s);
  int qsort_nat_ci_cmp(const void *A, const void *B); /* natural case-insensitive sort comparator */

  /*------------------------------ Filesystem ----------------------------------*/
  /* file_exists/dir_exists: return 1 if exists, else 0 (no errno dance needed). */
  int file_exists(const char *path);
  int dir_exists(const char *path);
  /* ueng_fopen: thin wrapper around fopen so future platform tweaks are centralized. */
  FILE *ueng_fopen(const char *path, const char *mode);

  /* NOTE: this prototype was missing earlier and caused link errors downstream */
  int write_text_file(const char *path, const char *content);

  int write_text_file_if_absent(const char *path, const char *content);
  int write_file(const char *path, const char *content);
  int copy_file_binary(const char *src, const char *dst);
  int write_gitkeep(const char *dir);
  int mkpath(const char *path);            /* mkdir -p */
  int mkpath_parent(const char *filepath); /* mkdir -p for parent dir only */
  int clean_dir(const char *dir);          /* rm -rf children */

  /*------------------------------ Time/format --------------------------------*/
  /* build_date_utc: "YYYY-MM-DD", build_timestamp_utc: "YYYY-MM-DDThh-mm-ssZ" */
  void build_date_utc(char *out, size_t outsz);
  void build_timestamp_utc(char *out, size_t outsz);
  /* slugify: normalize a title to a URL/dir-friendly slug */
  void slugify(const char *title, char *out, size_t outsz);

  /*------------------------------ Strings ------------------------------------*/
  /* Small helpers used by parsers and template writers. */
  char *ltrim(char *s);
  void rtrim(char *s);
  void unquote(char *s);
  void str_replace_inplace(char *buf, size_t bufsz, const char *pat, const char *rep);

  /*------------------------------ Exec/helpers -------------------------------*/
  /* exec_cmd: uses system(); returns 0 on success, 1 on non-zero exit, -1 on OS error. */
  int exec_cmd(const char *cmdline);
  /* path_abs: resolve to absolute path; path_to_file_url: make file:// URL browsers understand. */
  int path_abs(const char *in, char *out, size_t outsz);
  void path_to_file_url(const char *abs, char *out, size_t outsz);
  /* open_in_browser: launch default browser for file or URL. */
  int open_in_browser(const char *path_or_url);

#ifdef _WIN32
  /* Optional: switch Windows console to UTF-8 (call early in main if desired). */
  void ueng_console_utf8(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* UENG_COMMON_H */
/*------------------------------ Implementation ------------------------------*/
