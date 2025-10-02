/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/common.c
 * Purpose: Platform shims, filesystem + string helpers, exec utilities
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab + contributors
 * License: MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif
#include "ueng/common.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <shellapi.h>
#include <shlwapi.h>
#include <windows.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> /* ensure time(), struct tm, gmtime_*(), strftime are declared */

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/*---------------------------- string list -----------------------------------*/
/* NOTE: StrList helpers are declared in the header; keep simple utilities here */
static char *s_dup(const char *s)
{
  if (!s)
    return NULL;
  size_t n = strlen(s);
  char *d = (char *)malloc(n + 1);
  if (!d)
    return NULL;
  memcpy(d, s, n + 1);
  return d;
}
void sl_init(StrList *s) { memset(s, 0, sizeof(*s)); }
void sl_free(StrList *s)
{
  if (!s)
    return;
  for (size_t i = 0; i < s->count; ++i)
    free(s->items[i]);
  free(s->items);
  memset(s, 0, sizeof(*s));
}
int sl_push(StrList *s, const char *str)
{
  if (!s || !str)
    return -1;
  char *d = s_dup(str);
  if (!d)
    return -1;
  char **ni = (char **)realloc(s->items, (s->count + 1) * sizeof(char *));
  if (!ni)
  {
    free(d);
    return -1;
  }
  s->items = ni;
  s->items[s->count++] = d;
  return 0;
}

/*------------------------------ small strings -------------------------------*/
int str_eq_ci(const char *a, const char *b)
{
  if (!a || !b)
    return a == b;
  while (*a && *b)
  {
    unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
    if (tolower(ca) != tolower(cb))
      return 0;
    ++a;
    ++b;
  }
  return *a == *b;
}

/* natural sort compare (case-insensitive), e.g., ch2 < ch10 */
static int natcmp_ci(const char *a, const char *b)
{
  size_t i = 0, j = 0;
  for (;;)
  {
    unsigned char ca = (unsigned char)a[i];
    unsigned char cb = (unsigned char)b[j];
    if (ca == '\0' && cb == '\0')
      return 0;
    if (isdigit(ca) && isdigit(cb))
    {
      unsigned long long va = 0, vb = 0;
      size_t ia = i, jb = j;
      while (isdigit((unsigned char)a[ia]))
      {
        va = va * 10ULL + (unsigned)(a[ia] - '0');
        ia++;
      }
      while (isdigit((unsigned char)b[jb]))
      {
        vb = vb * 10ULL + (unsigned)(b[jb] - '0');
        jb++;
      }
      if (va < vb)
        return -1;
      if (va > vb)
        return 1;
      i = ia;
      j = jb;
      continue;
    }
    unsigned char ta = (unsigned char)tolower(ca);
    unsigned char tb = (unsigned char)tolower(cb);
    if (ta < tb)
      return -1;
    if (ta > tb)
      return 1;
    if (ca != '\0')
      i++;
    if (cb != '\0')
      j++;
  }
}
int qsort_nat_ci_cmp(const void *A, const void *B)
{
  const char *a = *(const char *const *)A;
  const char *b = *(const char *const *)B;
  return natcmp_ci(a, b);
}

/* slugify: safe ASCII-only slug for folder names */
void slugify(const char *in, char *out, size_t out_sz)
{
  if (!in || !out || out_sz == 0)
  {
    return;
  }
  size_t j = 0;
  int need_dash = 0;
  for (const unsigned char *p = (const unsigned char *)in; *p && j + 1 < out_sz; ++p)
  {
    unsigned char c = *p;
    if (isalnum(c))
    {
      out[j++] = (char)tolower(c);
      need_dash = 0;
    }
    else if (c == ' ' || c == '_' || c == '-' || c == '.')
    {
      if (!need_dash && j > 0)
      {
        out[j++] = '-';
        need_dash = 1;
      }
    }
    else
    {
      if (!need_dash && j > 0)
      {
        out[j++] = '-';
        need_dash = 1;
      }
    }
  }
  while (j > 0 && out[j - 1] == '-')
    j--;
  out[j] = '\0';
}

/*------------------------------ Filesystem ----------------------------------*/
int file_exists(const char *p)
{
#ifdef _WIN32
  DWORD a = GetFileAttributesA(p);
  return (a != INVALID_FILE_ATTRIBUTES) && !(a & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat st;
  return (stat(p, &st) == 0) && S_ISREG(st.st_mode);
#endif
}
int dir_exists(const char *p)
{
#ifdef _WIN32
  DWORD a = GetFileAttributesA(p);
  return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_DIRECTORY);
#else
  struct stat st;
  return (stat(p, &st) == 0) && S_ISDIR(st.st_mode);
#endif
}

/* DO NOT re-define make_dir here — it's implemented inline in the header on your tree */

/* Create all components of a path (portable) */
int mkpath(const char *p)
{
  if (!p || !*p)
    return -1;
#ifdef _WIN32
  char tmp[PATH_MAX];
  strncpy(tmp, p, sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  for (char *q = tmp; *q; ++q)
    if (*q == '/')
      *q = '\\';
  for (char *q = tmp + 1; *q; ++q)
  {
    if (*q == '\\')
    {
      char save = *q;
      *q = '\0';
      if (!dir_exists(tmp))
        if (make_dir(tmp) != 0 && !dir_exists(tmp))
          return -1;
      *q = save;
    }
  }
  if (!dir_exists(tmp))
    if (make_dir(tmp) != 0 && !dir_exists(tmp))
      return -1;
  return 0;
#else
  char tmp[PATH_MAX];
  strncpy(tmp, p, sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  for (char *q = tmp + 1; *q; ++q)
  {
    if (*q == '/')
    {
      *q = '\0';
      if (!dir_exists(tmp))
        if (mkdir(tmp, 0755) != 0 && !dir_exists(tmp))
          return -1;
      *q = '/';
    }
  }
  if (!dir_exists(tmp))
    if (mkdir(tmp, 0755) != 0 && !dir_exists(tmp))
      return -1;
  return 0;
#endif
}

int mkpath_parent(const char *path)
{
  if (!path || !*path)
    return -1;
  char buf[PATH_MAX];
  strncpy(buf, path, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  for (int i = (int)strlen(buf) - 1; i >= 0; --i)
  {
#ifdef _WIN32
    if (buf[i] == '\\' || buf[i] == '/')
    {
#else
    if (buf[i] == '/')
    {
#endif
      buf[i] = '\0';
      return mkpath(buf);
    }
  }
  return 0;
}

FILE *ueng_fopen(const char *path, const char *mode)
{
#ifdef _WIN32
  wchar_t wpath[PATH_MAX], wmode[16];
  MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, PATH_MAX);
  MultiByteToWideChar(CP_UTF8, 0, mode, -1, wmode, 16);
  return _wfopen(wpath, wmode);
#else
  return fopen(path, mode);
#endif
}

int write_text_file(const char *path, const char *text)
{
  if (mkpath_parent(path) != 0)
    return -1;
  FILE *f = ueng_fopen(path, "wb");
  if (!f)
    return -1;
  size_t n = fwrite(text, 1, strlen(text), f);
  fclose(f);
  return (n == strlen(text)) ? 0 : -1;
}
int write_text_file_if_absent(const char *path, const char *text)
{
  if (file_exists(path))
    return 0;
  return write_text_file(path, text ? text : "");
}
int read_text_file(const char *path, char *out, size_t outsz)
{
  FILE *f = ueng_fopen(path, "rb");
  if (!f)
    return -1;
  size_t n = fread(out, 1, outsz - 1, f);
  fclose(f);
  out[n] = '\0';
  return 0;
}

/* Copy file binary (makes parent dir) */
int copy_file_binary(const char *src, const char *dst)
{
  if (!src || !dst)
    return -1;
  FILE *in = ueng_fopen(src, "rb");
  if (!in)
    return -1;
  if (mkpath_parent(dst) != 0)
  {
    fclose(in);
    return -1;
  }
  FILE *out = ueng_fopen(dst, "wb");
  if (!out)
  {
    fclose(in);
    return -1;
  }
  char buf[64 * 1024];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
  {
    if (fwrite(buf, 1, n, out) != n)
    {
      fclose(in);
      fclose(out);
      return -1;
    }
  }
  fclose(in);
  fclose(out);
  return 0;
}

/* Ensure dir and write empty .gitkeep */
int write_gitkeep(const char *dir)
{
  if (!dir || !*dir)
    return -1;
  if (mkpath(dir) != 0)
    return -1;
  char p[PATH_MAX];
  snprintf(p, sizeof(p), "%s%c.gitkeep", dir, PATH_SEP);
  return write_text_file_if_absent(p, "");
}

/* Clean directory contents, keep the directory itself */
int clean_dir(const char *dir)
{
  if (!dir || !*dir)
    return -1;
  if (!dir_exists(dir))
    return 0;
#ifdef _WIN32
  char pattern[PATH_MAX];
  snprintf(pattern, sizeof(pattern), "%s\\*.*", dir);
  WIN32_FIND_DATAA f;
  HANDLE h = FindFirstFileA(pattern, &f);
  if (h == INVALID_HANDLE_VALUE)
    return 0;
  do
  {
    const char *n = f.cFileName;
    if (strcmp(n, ".") == 0 || strcmp(n, "..") == 0)
      continue;
    char p[PATH_MAX];
    snprintf(p, sizeof(p), "%s\\%s", dir, n);
    if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      clean_dir(p);
      RemoveDirectoryA(p);
    }
    else
    {
      if (f.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
        SetFileAttributesA(p, f.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
      }
      DeleteFileA(p);
    }
  } while (FindNextFileA(h, &f));
  FindClose(h);
  return 0;
#else
  DIR *d = opendir(dir);
  if (!d)
    return 0;
  struct dirent *e;
  while ((e = readdir(d)))
  {
    const char *n = e->d_name;
    if (strcmp(n, ".") == 0 || strcmp(n, "..") == 0)
      continue;
    char p[PATH_MAX];
    snprintf(p, sizeof(p), "%s/%s", dir, n);
    struct stat st;
    if (stat(p, &st) != 0)
      continue;
    if (S_ISDIR(st.st_mode))
    {
      clean_dir(p);
      rmdir(p);
    }
    else
      unlink(p);
  }
  closedir(d);
  return 0;
#endif
}

/*------------------------------ Exec/helpers --------------------------------*/
int exec_cmd(const char *cmdline)
{
#ifdef _WIN32
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  char cmd[32768];
  strncpy(cmd, cmdline, sizeof(cmd) - 1);
  cmd[sizeof(cmd) - 1] = '\0';
  if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
  {
    return -1;
  }
  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD code = 0;
  GetExitCodeProcess(pi.hProcess, &code);
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return (int)code;
#else
  int rc = system(cmdline);
  if (rc == -1)
    return -1;
  if (WIFEXITED(rc))
    return WEXITSTATUS(rc);
  return -1;
#endif
}

int path_abs(const char *in, char *out, size_t outsz)
{
#ifdef _WIN32
  wchar_t wi[PATH_MAX], wo[PATH_MAX];
  MultiByteToWideChar(CP_UTF8, 0, in, -1, wi, PATH_MAX);
  DWORD n = GetFullPathNameW(wi, PATH_MAX, wo, NULL);
  if (n == 0 || n >= PATH_MAX)
    return -1;
  WideCharToMultiByte(CP_UTF8, 0, wo, -1, out, (int)outsz, NULL, NULL);
  return 0;
#else
  if (!realpath(in, out))
    return -1;
  return 0;
#endif
}

void path_to_file_url(const char *abs, char *out, size_t outsz)
{
#ifdef _WIN32
  char tmp[PATH_MAX];
  strncpy(tmp, abs, sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  for (char *p = tmp; *p; ++p)
    if (*p == '\\')
      *p = '/';
  snprintf(out, outsz, "file:///%s", tmp);
#else
  snprintf(out, outsz, "file://%s", abs);
#endif
}

int browse_file_or_url(const char *what)
{
#ifdef _WIN32
  return (int)(intptr_t)ShellExecuteA(NULL, "open", what, NULL, NULL, SW_SHOWNORMAL) > 32 ? 0 : -1;
#else
  char cmd[PATH_MAX + 64];
#ifdef __APPLE__
  snprintf(cmd, sizeof(cmd), "open '%s'", what);
#else
  snprintf(cmd, sizeof(cmd), "xdg-open '%s' >/dev/null 2>&1", what);
#endif
  return exec_cmd(cmd);
#endif
}

/* --- Added for main.c link errors --- */
void build_date_utc(char *out, size_t out_sz)
{
  if (!out || out_sz == 0)
  {
    return;
  }
  time_t t = time(NULL);
  struct tm g;
#ifdef _WIN32
  gmtime_s(&g, &t);
#else
  gmtime_r(&t, &g);
#endif
  if (out_sz >= 11)
  {
    strftime(out, out_sz, "%Y-%m-%d", &g);
  }
  else
  {
    out[0] = '\0';
  }
}

int open_in_browser(const char *path_or_url)
{
  if (!path_or_url || !*path_or_url)
    return -1;
  /* If it's already a URL, just open it */
  if (strstr(path_or_url, "://") != NULL)
  {
    return browse_file_or_url(path_or_url);
  }
  /* Otherwise convert an absolute path to file:// URL and open */
  char abs[PATH_MAX], url[PATH_MAX + 16];
  if (path_abs(path_or_url, abs, sizeof(abs)) != 0)
    return -1;
  path_to_file_url(abs, url, sizeof(url));
  return browse_file_or_url(url);
}
/* --- end additions --- */

#ifdef _WIN32
void ueng_console_utf8(void)
{
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
}
#endif

/*------------------------------ End of file --------------------------------*/