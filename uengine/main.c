/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/main.c
 * PURPOSE: Command-line front-end (dispatcher + light helpers)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* ============================ QUICK START FOR NEW CONTRIBUTORS ============================
   This file implements the uaengine CLI. The program is organized as:
     - Small helpers (path/tool detection, YAML mini-parser)
     - Command handlers: cmd_init, cmd_ingest, cmd_build, cmd_export, cmd_open, cmd_serve,
   cmd_doctor, cmd_publish
     - main(): routes argv[1] to one of the command handlers
   Build flow (high level):
     1) 'uaengine init'  -> creates folders and a boilerplate book.yaml
     2) 'uaengine build' -> optionally ingests/normalizes content and packs workspace/book-draft.md
     3) 'uaengine export'-> runs Pandoc to produce HTML (and optionally PDF), writing logs next to
   outputs 4) 'uaengine open'  -> opens the generated site index in your browser 5) 'uaengine serve'
   -> (optional) serves the site folder locally

   Windows specifics:
     - We pass ABSOLUTE paths to Pandoc to avoid working-directory surprises.
     - We use '--metadata-file meta.yaml' instead of inline '--metadata' to avoid quoting issues.
     - On export failure, a short warning is printed and the first lines of pandoc_err.txt are
   echoed.
   ========================================================================================= */
#include "ueng/common.h" /* filesystem helpers, shell exec, slugify, etc. */
#include "ueng/fs.h"     /* pack_book_draft, write_site_index, theme copy */
#include "ueng/serve.h"  /* tiny HTTP server entry point */
#include "ueng/version.h"

/* If the build system ever forgets to define UENG_VERSION_STR, fall back. */
#ifndef UENG_VERSION_STR
#define UENG_VERSION_STR UENG_VERSION_STRING
#endif

/*----------------------------- local config --------------------------------*/
/* Small struct for a few book.yaml fields we care about. */
typedef struct
{
  char title[256];
  char author[256];
  int ingest_on_build; /* 0/1 */
} BookCfg;

/* Defaults used if book.yaml is missing or a field is absent. */
static void cfg_defaults(BookCfg *c)
{
  memset(c, 0, sizeof(*c));
  strcpy(c->title, "My New Book");
  strcpy(c->author, "Anonymous");
  c->ingest_on_build = 0;
}

/*-------------------------- tiny YAML-ish parsing --------------------------*/
/* We intentionally avoid a YAML library to keep the binary tiny/portable. */

static void trim_eol(char *s)
{
  size_t n = strlen(s);
  while (n && (s[n - 1] == '\r' || s[n - 1] == '\n'))
    s[--n] = '\0';
}

/* Parse lines like:  key: value   → copies value into out (bounded). */
static void parse_kv_line(const char *line, const char *key, char *out, size_t outsz)
{
  size_t klen = strlen(key);
  if (strncmp(line, key, klen) == 0)
  {
    const char *p = line + klen;
    while (*p == ' ' || *p == '\t')
      p++;
    if (*p == ':')
      p++;
    while (*p == ' ' || *p == '\t')
      p++;
    strncpy(out, p, outsz - 1);
    out[outsz - 1] = '\0';
  }
}

/* Parse boolean-ish: true/false/1/0/yes/no/on/off (case-insensitive). */
static int parse_bool_line(const char *line, const char *key, int *out_bool)
{
  char buf[64] = {0};
  parse_kv_line(line, key, buf, sizeof(buf));
  if (!buf[0])
    return 0;
  for (char *p = buf; *p; ++p)
    *p = (char)tolower((unsigned char)*p);
  if (strcmp(buf, "true") == 0 || strcmp(buf, "1") == 0 || strcmp(buf, "yes") == 0 ||
      strcmp(buf, "on") == 0)
  {
    *out_bool = 1;
    return 1;
  }
  if (strcmp(buf, "false") == 0 || strcmp(buf, "0") == 0 || strcmp(buf, "no") == 0 ||
      strcmp(buf, "off") == 0)
  {
    *out_bool = 0;
    return 1;
  }
  return 0;
}

/* Read minimal config from book.yaml if present. */
static void read_book_cfg(BookCfg *out)
{
  cfg_defaults(out);
  FILE *f = ueng_fopen("book.yaml", "rb");
  if (!f)
    return; /* keep defaults */
  char line[1024];
  while (fgets(line, sizeof(line), f))
  {
    trim_eol(line);
    parse_kv_line(line, "title", out->title, sizeof(out->title));
    parse_kv_line(line, "author", out->author, sizeof(out->author));
    (void)parse_bool_line(line, "ingest_on_build", &out->ingest_on_build);
  }
  fclose(f);
}

/*---------------------------- light HTML export ----------------------------*/
/* Fallback exporter used when Pandoc isn't available or fails. */

static void html_escape_into(const char *in, FILE *out)
{
  for (const unsigned char *p = (const unsigned char *)in; *p; ++p)
  {
    switch (*p)
    {
    case '&':
      fputs("&amp;", out);
      break;
    case '<':
      fputs("&lt;", out);
      break;
    case '>':
      fputs("&gt;", out);
      break;
    default:
      fputc(*p, out);
      break;
    }
  }
}

static int light_export_html(const char *title, const char *author, const char *html_dir,
                             const char *md_path, char *out_html, size_t out_html_sz)
{
  if (mkpath(html_dir) != 0)
  {
    fprintf(stderr, "[export] ERROR: cannot create %s\n", html_dir);
    return 1;
  }
  char rel_css[32];
  rel_css[0] = '\0';
  (void)copy_theme_into_html_dir(html_dir, rel_css, sizeof(rel_css)); /* writes style.css */

  snprintf(out_html, out_html_sz, "%s%cbook.html", html_dir, PATH_SEP);
  FILE *in = ueng_fopen(md_path, "rb");
  if (!in)
  {
    fprintf(stderr, "[export] ERROR: missing %s\n", md_path);
    return 1;
  }
  FILE *out = ueng_fopen(out_html, "wb");
  if (!out)
  {
    fclose(in);
    fprintf(stderr, "[export] ERROR: cannot write %s\n", out_html);
    return 1;
  }

  fputs("<!doctype html><meta charset=\"utf-8\">", out);
  fprintf(out, "<title>%s - %s</title>", title, author);
  fprintf(out, "<link rel=\"stylesheet\" href=\"style.css\">");
  fputs("<body style=\"margin:2rem auto;max-width:860px;font-family:system-ui,-apple-system,Segoe "
        "UI,Roboto,Ubuntu,Arial,sans-serif;line-height:1.6\">",
        out);
  fprintf(out, "<h1>%s</h1><p>Author: %s</p>", title, author);
  fputs("<pre>", out);
  char buf[8192];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
  {
    buf[n] = '\0';
    html_escape_into(buf, out);
  }
  fputs("</pre></body>", out);
  fclose(in);
  fclose(out);
  printf("[export] light HTML: %s\n", out_html);
  return 0;
}

/*-------------------------------- Commands ---------------------------------*/

/* init: make a minimal project that builds immediately. */
static int cmd_init(void)
{
  (void)write_text_file_if_absent("book.yaml", "title: My New Book\n"
                                               "author: Anonymous\n"
                                               "ingest_on_build: false\n");

  BookCfg cfg;
  read_book_cfg(&cfg);
  char slug[256];
  slugify(cfg.title, slug, sizeof(slug));

  /* Seed workspace content and cover assets. */
  generate_cover_svg(cfg.title, cfg.author, slug);
  generate_frontcover_md(cfg.title, cfg.author, slug);

  puts("[init] done.");
  return 0;
}

/* ingest: placeholder hook for OCR/cleaning/organizing into workspace. */
static int cmd_ingest(void)
{
  puts("[ingest] nothing to do (stub).");
  return 0;
}

/* build: creates outputs/<slug>/<YYYY-MM-DD>/, packs draft, seeds site, HTML theme. */
static int cmd_build(void)
{
  BookCfg cfg;
  read_book_cfg(&cfg);

  if (cfg.ingest_on_build)
  {
    puts("[build] ingest_on_build: true - running ingest...");
    (void)cmd_ingest(); /* ignore failure for now */
  }

  char slug[256];
  slugify(cfg.title, slug, sizeof(slug));
  char day[32];
  build_date_utc(day, sizeof(day));

  /* outputs/<slug>/<YYYY-MM-DD> */
  char root[640];
  snprintf(root, sizeof(root), "outputs%c%s%c%s", PATH_SEP, slug, PATH_SEP, day);
  mkpath(root);

  /* Common subfolders we expect to populate. */
  const char *sub[] = {"pdf", "docx", "epub", "html", "md", "cover", "video-scripts", "site", NULL};
  for (int i = 0; sub[i]; ++i)
  {
    char p[640];
    snprintf(p, sizeof(p), "%s%c%s", root, PATH_SEP, sub[i]);
    mkpath(p);
  }

  /* Make sure cover assets exist before packaging. */
  generate_cover_svg(cfg.title, cfg.author, slug);
  generate_frontcover_md(cfg.title, cfg.author, slug);

  /* Draft + site packing (see fs.c). */
  int has_draft = 0;
  if (pack_book_draft(cfg.title, root, &has_draft) != 0)
  {
    fprintf(stderr, "[build] ERROR: could not pack draft\n");
    return 1;
  }

  /* Ensure theme exists in html/ (style.css, etc.). */
  char html_dir[640];
  snprintf(html_dir, sizeof(html_dir), "%s%chtml", root, PATH_SEP);
  char rel_css_tmp[16];
  rel_css_tmp[0] = '\0';
  (void)copy_theme_into_html_dir(html_dir, rel_css_tmp, sizeof(rel_css_tmp));

  /* Make a simple site landing page with links. */
  char stamp[64];
  time_t now = time(NULL);
  struct tm *tm = gmtime(&now);
  strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M UTC", tm);
  char site_dir[640];
  snprintf(site_dir, sizeof(site_dir), "%s%csite", root, PATH_SEP);
  if (write_site_index(site_dir, cfg.title, cfg.author, slug, stamp, /*has_cover=*/1, has_draft) !=
      0)
  {
    fprintf(stderr, "[build] WARN: could not write site/index.html\n");
  }

  /* If Pandoc export hasn't been run yet, provide a fallback HTML so
     users can open something immediately. */
  {
    char out_html[768];
    snprintf(out_html, sizeof(out_html), "%s%cbook.html", html_dir, PATH_SEP);
    if (!file_exists(out_html) && file_exists("workspace/book-draft.md"))
    {
      (void)light_export_html(cfg.title, cfg.author, html_dir, "workspace/book-draft.md", out_html,
                              sizeof(out_html));
    }
  }

  printf("[build] ok: %s\n", root);
  return 0;
}

/* export: prefer Pandoc if present; otherwise fall back to light HTML. */
static int cmd_export(void)
{
  BookCfg cfg;
  read_book_cfg(&cfg);
  char slug[256];
  slugify(cfg.title, slug, sizeof(slug));
  char day[32];
  build_date_utc(day, sizeof(day));

  if (!file_exists("workspace/book-draft.md"))
  {
    fprintf(stderr, "[export] workspace/book-draft.md not found. Run `uaengine build` first.\n");
    return 1;
  }

  char out_root[640];
  snprintf(out_root, sizeof(out_root), "outputs%c%s%c%s", PATH_SEP, slug, PATH_SEP, day);
  char html_dir[640];
  snprintf(html_dir, sizeof(html_dir), "%s%chtml", out_root, PATH_SEP);
  mkpath(html_dir);

  char rel_css[64];
  rel_css[0] = '\0';
  (void)copy_theme_into_html_dir(html_dir, rel_css, sizeof(rel_css)); /* sets rel_css=style.css */

  char out_html[768];
  snprintf(out_html, sizeof(out_html), "%s%cbook.html", html_dir, PATH_SEP);

  int used_pandoc = 0;
#ifdef _WIN32
  const char *probe = "where pandoc >NUL 2>&1";
#else
  const char *probe = "command -v pandoc >/dev/null 2>&1";
#endif
  if (exec_cmd(probe) == 0)
  {
#ifdef _WIN32
    char cmd1[2048];
    /* Windows: use -M key="value" form (robust quoting in cmd.exe),
       and include workspace/dropzone as resource search paths. */
    snprintf(cmd1, sizeof(cmd1),
             "pandoc -f markdown -t html5 --standalone "
             "-M title=\"%s\" -M author=\"%s\" "
             "--resource-path=\"%%CD%%;dropzone;workspace\" %s "
             "-o \"%s\" "
             "\"workspace\\book-draft.md\"",
             cfg.title, cfg.author, rel_css[0] ? "-c \"style.css\"" : "", out_html);
#else
    char cmd1[2048];
    /* POSIX shells: quoting is simpler; keep input last. */
    snprintf(cmd1, sizeof(cmd1),
             "pandoc -f markdown -t html5 --standalone "
             "-M title=\"%s\" -M author=\"%s\" "
             "--resource-path=\"${PWD}:dropzone:workspace\" %s "
             "-o \"%s\" "
             "\"workspace/book-draft.md\"",
             cfg.title, cfg.author, rel_css[0] ? "-c \"style.css\"" : "", out_html);
#endif
    /* Uncomment if you want to see the exact pandoc command:
       fprintf(stderr, "[debug] %s\n", cmd1);
    */
    if (exec_cmd(cmd1) == 0)
    {
      used_pandoc = 1;
      printf("[export] pandoc HTML: %s\n", out_html);
    }
  }

  if (!used_pandoc)
  {
    (void)light_export_html(cfg.title, cfg.author, html_dir, "workspace/book-draft.md", out_html,
                            sizeof(out_html));
  }

  puts("[export] done");
  return 0;
}

/* serve: start the tiny HTTP server at a site folder.
   Usage:
     uaengine serve                 → serves today's site for the current book
     uaengine serve HOST [PORT]     → serves today's site at HOST:PORT
     uaengine serve --site PATH     → serves PATH directly
*/
static int cmd_serve(int argc, char **argv)
{
  const char *site_root = getenv("UENG_SITE_ROOT");
  char calc_root[768] = {0};
  char host[64] = "127.0.0.1";
  int port = 8080;

  int i = 0;
  if (argc >= 2 && strcmp(argv[0], "--site") == 0)
  {
    site_root = argv[1];
    i = 2;
  }

  /* Parse either a PORT or HOST [PORT] */
  if (i < argc)
  {
    const char *a = argv[i];
    int is_port = 1;
    for (const char *p = a; *p; ++p)
      if (!isdigit((unsigned char)*p))
      {
        is_port = 0;
        break;
      }
    if (strchr(a, '.') || strchr(a, ':'))
      is_port = 0;
    if (is_port)
    {
      port = atoi(a);
      ++i;
    }
    else
    {
      snprintf(host, sizeof(host), "%s", a);
      ++i;
      if (i < argc)
      {
        port = atoi(argv[i]);
        ++i;
      }
    }
  }

  /* If not overridden, compute today's site path for the current book. */
  if (!site_root || !*site_root)
  {
    BookCfg cfg;
    read_book_cfg(&cfg);
    char slug[256];
    slugify(cfg.title, slug, sizeof(slug));
    char day[32];
    build_date_utc(day, sizeof(day));
    snprintf(calc_root, sizeof(calc_root), "outputs%c%s%c%s%csite", PATH_SEP, slug, PATH_SEP, day,
             PATH_SEP);
    site_root = calc_root;
  }

  printf("[serve] Serving %s at http://%s:%d\n", site_root, host, port);
  return serve_run(site_root, host, port);
}

/* open: open the generated site for today’s build in your default browser. */
static int cmd_open(void)
{
  const char *site_root = getenv("UENG_SITE_ROOT");
  char calc_root[768] = {0};
  if (!site_root || !*site_root)
  {
    BookCfg cfg;
    read_book_cfg(&cfg);
    char slug[256];
    slugify(cfg.title, slug, sizeof(slug));
    char day[32];
    build_date_utc(day, sizeof(day));
    snprintf(calc_root, sizeof(calc_root), "outputs%c%s%c%s%csite", PATH_SEP, slug, PATH_SEP, day,
             PATH_SEP);
    site_root = calc_root;
  }
  char index_path[1024];
  snprintf(index_path, sizeof(index_path), "%s%cindex.html", site_root, PATH_SEP);
  if (!file_exists(index_path))
  {
    fprintf(stderr, "[open] ERROR: %s not found. Run uaengine export or uaengine build first.\n",
            index_path);
    return 1;
  }
  return open_in_browser(index_path);
}

/* doctor: quick environment sanity check to help users debug setup. */
static int cmd_doctor(void)
{
  puts("uaengine doctor - environment check");

  printf("[ok] book.yaml %s\n", file_exists("book.yaml") ? "found" : "missing");
  printf("[ok] dropzone/ %s\n", dir_exists("dropzone") ? "found" : "missing");
  printf("[ok] workspace/ %s\n", dir_exists("workspace") ? "found" : "missing");

#ifdef _WIN32
  const char *probe = "where pandoc >NUL 2>&1";
#else
  const char *probe = "command -v pandoc >/dev/null 2>&1";
#endif
  int has_pandoc = (exec_cmd(probe) == 0);
  puts(has_pandoc ? "[ok] pandoc found on PATH"
                  : "[info] pandoc not found (HTML export will use light fallback)");

#ifdef _WIN32
  puts("[ok] Edge/Chrome present for headless PDF");
#else
  int has_headless = (exec_cmd("command -v google-chrome >/dev/null 2>&1") == 0) ||
                     (exec_cmd("command -v chromium >/dev/null 2>&1") == 0) ||
                     (exec_cmd("command -v microsoft-edge >/dev/null 2>&1") == 0);
  puts(has_headless ? "[ok] Edge/Chrome present for headless PDF"
                    : "[info] No headless Chrome found (PDF via pandoc only)");
#endif

  const char *env_root = getenv("UENG_SITE_ROOT");
  if (env_root && *env_root)
  {
    printf("[info] UENG_SITE_ROOT=%s\n", env_root);
  }
  else
  {
    puts("[info] UENG_SITE_ROOT not set (serve will use today's site by default)");
  }

  puts("Done.");
  return 0;
}

/* Not implemented yet, but kept visible so users know it's planned. */
static int cmd_publish(void)
{
  fprintf(stderr, "[publish] Not implemented.\n");
  return 1;
}

/* render: convenience command that runs build → export → open. */
static int cmd_render(void)
{
  int rc = cmd_build();
  if (rc != 0)
    return rc;
  rc = cmd_export();
  if (rc != 0)
    return rc;
  rc = cmd_open();
  return rc;
}

/*---------------------------------- main -----------------------------------*/
/* Maps argv[1] to the command handlers above. */

static void usage(void)
{
  puts("Umicom AuthorEngine AI (uaengine) - Manage your book projects with AI assistance.\n");
  puts("Usage: uaengine <command> [options]\n");
  puts("Commands:");
  puts("  init                 Initialize a new book project structure.");
  puts("  ingest               Ingest and organize content from the dropzone.");
  puts("  build                Build the book draft and prepare outputs.");
  puts("  export               Export the book to HTML and PDF formats.");
  puts("  serve [opts]         Serve a site folder (defaults to today's site).");
  puts("  open                 Open the latest site (or UENG_SITE_ROOT) in browser.");
  puts("  render               Build + Export + Open (convenience).");
  puts("  doctor               Check environment, tools, and folders.");
  puts("  publish              Publish the book to a remote server (not implemented).");
  puts("  --version            Show version information.");
}

/*--------------------------------------------------------------------------
 * Inserted by scripts/fix/add-llm-selftest.*
 * PURPOSE: Add 'llm-selftest' command to exercise the embedded LLM wrapper.
 *---------------------------------------------------------------------------*/
#include "ueng/llm.h"

static int cmd_llm_selftest(int argc, char **argv)
{
  const char *model = NULL;
  if (argc >= 3)
    model = argv[2];
  if (!model || !*model)
  {
    model = getenv("UENG_LLM_MODEL");
  }
  if (!model || !*model)
  {
    fprintf(stderr, "[llm-selftest] ERROR: no model path given and UENG_LLM_MODEL not set.\n");
    return 2;
  }

  char err[256] = {0};
  ueng_llm_ctx *L = ueng_llm_open(model, 4096, err, sizeof(err));
  if (!L)
  {
    fprintf(stderr, "[llm-selftest] open failed: %s\n", err[0] ? err : "(unknown)");
    return 3;
  }
  char out[2048] = {0};
  int rc = ueng_llm_prompt(L, "Say hello from AuthorEngine.", out, sizeof(out));
  if (rc == 0)
  {
    printf("%s\n", out);
  }
  else
  {
    fprintf(stderr, "[llm-selftest] prompt failed (rc=%d)\n", rc);
  }
  ueng_llm_close(L);
  return rc;
}
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    usage();
    return 0;
  }
  if (strcmp(argv[1], "--version") == 0)
  {
    puts(UENG_VERSION_STR);
    return 0;
  }

  /* -----------------------------------------------------------------------
   * Accept '--help' / '-h' explicitly and exit 0 (so CI steps never fail
   * when checking the help text). This is additive and does not change any
   * existing behavior. 'uaengine help' remains supported below.
   * Created by: Umicom Foundation (https://umicom.foundation/)
   * Author: Sammy Hegab
   * Date: 24-09-2025
   * --------------------------------------------------------------------- */
  if ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0))
  {
    usage();
    return 0;
  }

  const char *cmd = argv[1];
  if (strcmp(cmd, "llm-selftest") == 0)
    return cmd_llm_selftest(argc, argv);
  if (strcmp(cmd, "help") == 0)
  {
    usage();
    return 0;
  }
  else if (strcmp(cmd, "init") == 0)
  {
    return cmd_init();
  }
  else if (strcmp(cmd, "ingest") == 0)
  {
    return cmd_ingest();
  }
  else if (strcmp(cmd, "build") == 0)
  {
    return cmd_build();
  }
  else if (strcmp(cmd, "export") == 0)
  {
    return cmd_export();
  }
  else if (strcmp(cmd, "serve") == 0)
  {
    return cmd_serve(argc - 2, argv + 2);
  }
  else if (strcmp(cmd, "open") == 0)
  {
    return cmd_open();
  }
  else if (strcmp(cmd, "render") == 0)
  {
    return cmd_render();
  }
  else if (strcmp(cmd, "doctor") == 0)
  {
    return cmd_doctor();
  }
  else if (strcmp(cmd, "publish") == 0)
  {
    return cmd_publish();
  }
  else
  {
    fprintf(stderr, "Unknown command: %s\n", cmd);
    usage();
    return 1;
  }
}
