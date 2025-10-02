/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/fs.c
 * Purpose: Filesystem helpers specific to uaengine (book packing, site files)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab + contributors
 * License: MIT
 *---------------------------------------------------------------------------*/
#include "ueng/fs.h"
#include "ueng/common.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* NOTE:
   -----
   StrList helpers (sl_init/sl_free/sl_push) now live in src/common.c.
   This file should NOT define them to avoid duplicate symbol link errors.
*/

/*------------------------------- Theme files --------------------------------*/

int copy_theme_into_html_dir(const char *html_dir, char *rel_css, size_t rel_css_sz)
{
  if (mkpath(html_dir) != 0)
    return -1;
  char css[PATH_MAX];
  snprintf(css, sizeof(css), "%s%cstyle.css", html_dir, PATH_SEP);
  const char *minimal_css = "body{color:#111;background:#fff}"
                            "h1,h2,h3{line-height:1.25}"
                            "pre{white-space:pre-wrap}";
  if (write_text_file_if_absent(css, minimal_css) != 0)
    return -1;
  if (rel_css && rel_css_sz)
  {
    strncpy(rel_css, "style.css", rel_css_sz - 1);
    rel_css[rel_css_sz - 1] = '\0';
  }
  return 0;
}

/*---------------------------- Chapter packaging -----------------------------*/

/* Concatenate *.md from `dir` in natural case-insensitive order. */
static int concat_md_dir(const char *dir, FILE *out)
{
  StrList list;
  sl_init(&list);

#ifdef _WIN32
  char pattern[PATH_MAX];
  snprintf(pattern, sizeof(pattern), "%s\\*.md", dir);
  WIN32_FIND_DATAA f;
  HANDLE h = FindFirstFileA(pattern, &f);
  if (h != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (f.cFileName[0] == '\0')
        continue;
      sl_push(&list, f.cFileName);
    } while (FindNextFileA(h, &f));
    FindClose(h);
  }
#else
  DIR *d = opendir(dir);
  if (d)
  {
    struct dirent *ent;
    while ((ent = readdir(d)))
    {
      const char *n = ent->d_name;
      size_t ln = n ? strlen(n) : 0;
      if (ln >= 4 && strcmp(n + ln - 3, ".md") == 0)
      {
        sl_push(&list, n);
      }
    }
    closedir(d);
  }
#endif

  /* Sort: natural, case-insensitive (e.g., ch2.md < ch10.md) */
  if (list.count > 1)
  {
    qsort(list.items, list.count, sizeof(char *), qsort_nat_ci_cmp);
  }

  for (size_t i = 0; i < list.count; ++i)
  {
    char p[PATH_MAX];
#ifdef _WIN32
    snprintf(p, sizeof(p), "%s\\%s", dir, list.items[i]);
#else
    snprintf(p, sizeof(p), "%s/%s", dir, list.items[i]);
#endif
    FILE *in = ueng_fopen(p, "rb");
    if (!in)
      continue;
    fprintf(out, "\n\n<!-- %s -->\n\n", list.items[i]);
    char buf[65536];
    size_t nrd;
    while ((nrd = fread(buf, 1, sizeof(buf), in)) > 0)
      fwrite(buf, 1, nrd, out);
    fclose(in);
  }

  sl_free(&list);
  return 0;
}

int pack_book_draft(const char *title, const char *outputs_root, int *out_has_draft)
{
  (void)outputs_root; /* draft always under workspace/ */
  (void)mkpath("workspace");
  char draft[PATH_MAX];
  snprintf(draft, sizeof(draft), "workspace%cbook-draft.md", PATH_SEP);
  FILE *out = ueng_fopen(draft, "wb");
  if (!out)
    return -1;

  fprintf(out, "# %s\n\n", title);
  /* frontmatter */
  if (file_exists("workspace/chapters/_frontmatter.md"))
  {
    FILE *in = ueng_fopen("workspace/chapters/_frontmatter.md", "rb");
    if (in)
    {
      char buf[65536];
      size_t nrd;
      while ((nrd = fread(buf, 1, sizeof(buf), in)) > 0)
        fwrite(buf, 1, nrd, out);
      fclose(in);
      fputs("\n\n", out);
    }
  }
  /* chapters (deterministic order) */
  (void)concat_md_dir("workspace/chapters", out);

  /* acknowledgements */
  if (file_exists("workspace/chapters/acknowledgements.md"))
  {
    FILE *in = ueng_fopen("workspace/chapters/acknowledgements.md", "rb");
    if (in)
    {
      fputs("\n\n", out);
      char buf[65536];
      size_t nrd;
      while ((nrd = fread(buf, 1, sizeof(buf), in)) > 0)
        fwrite(buf, 1, nrd, out);
      fclose(in);
    }
  }

  fclose(out);
  if (out_has_draft)
    *out_has_draft = 1;
  return 0;
}

/*------------------------------- Cover helpers ------------------------------*/

int generate_cover_svg(const char *title, const char *author, const char *slug)
{
  (void)slug;
  (void)mkpath("workspace");
  char path[PATH_MAX];
  snprintf(path, sizeof(path), "workspace%ccover.svg", PATH_SEP);
  char buf[4096];
  snprintf(buf, sizeof(buf),
           "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1200\" height=\"1600\">\n"
           "<rect width=\"100%%\" height=\"100%%\" fill=\"#f4f4f4\"/>\n"
           "<text x=\"50\" y=\"200\" font-size=\"64\" font-family=\"Segoe "
           "UI,Arial,sans-serif\">%s</text>\n"
           "<text x=\"50\" y=\"300\" font-size=\"32\" font-family=\"Segoe UI,Arial,sans-serif\">by "
           "%s</text>\n"
           "</svg>\n",
           title, author);
  return write_text_file(path, buf);
}

int generate_frontcover_md(const char *title, const char *author, const char *slug)
{
  (void)slug;
  (void)mkpath("workspace");
  char path[PATH_MAX];
  snprintf(path, sizeof(path), "workspace%cfrontcover.md", PATH_SEP);
  char buf[2048];
  snprintf(buf, sizeof(buf),
           "# %s\n\n"
           "_by %s_\n\n"
           "![cover](cover.svg)\n",
           title, author);
  return write_text_file(path, buf);
}

/*------------------------------- Site helpers -------------------------------*/

int write_site_index(const char *site_dir, const char *title, const char *author, const char *slug,
                     const char *stamp, int has_cover, int has_draft)
{
  if (mkpath(site_dir) != 0)
    return -1;

  char html[PATH_MAX];
  snprintf(html, sizeof(html), "%s%cindex.html", site_dir, PATH_SEP);
  char book_html_rel[PATH_MAX];
  snprintf(book_html_rel, sizeof(book_html_rel), "..%chtml%cbook.html", PATH_SEP, PATH_SEP);

  char buf[8192];
  size_t used = 0;
  int n = snprintf(
      buf + used, sizeof(buf) - used,
      "<!doctype html>\n<meta charset=\"utf-8\">\n<title>%s</title>\n"
      "<link rel=\"stylesheet\" href=\"../html/style.css\">\n"
      "<body style=\"margin:2rem auto;max-width:860px;font-family:system-ui,-apple-system,Segoe "
      "UI,Roboto,Ubuntu,Arial,sans-serif;line-height:1.6\">\n"
      "<main>\n<h1>%s</h1>\n<p>Author: %s</p>\n<p><small>%s</small></p>\n",
      title, title, author, stamp);
  if (n < 0)
    return -1;
  used += (size_t)n;

  if (has_cover)
  {
    n = snprintf(
        buf + used, sizeof(buf) - used,
        "<p><img src=\"../cover/cover.svg\" alt=\"Cover\" style=\"max-width:240px\"></p>\n");
    if (n < 0)
      return -1;
    used += (size_t)n;
  }
  if (has_draft)
  {
    n = snprintf(buf + used, sizeof(buf) - used, "<p><a href=\"%s\">Read HTML Draft</a></p>\n",
                 book_html_rel);
    if (n < 0)
      return -1;
    used += (size_t)n;
  }

  n = snprintf(buf + used, sizeof(buf) - used,
               "<p>Generated by Umicom AuthorEngine AI.</p>\n"
               "</main>\n</body></html>\n");
  if (n < 0)
    return -1;

  /* Use the canonical write_text_file from common.c */
  return write_text_file(html, buf);
}
/*------------------------------ Path helpers -------------------------------*/
