/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: include/ueng/fs.h
 * Purpose: Filesystem helpers specific to uaengine (book packing, site files)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab + contributors
 * License: MIT
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Module notes (added, non-destructive):
 * MODULE OVERVIEW
 * ----------------
 * Purpose: Higher-level filesystem helpers specific to the authoring workflow.
 * Responsibilities:
 *   - Writing minimal HTML 'index' pages for exported sites.
 *   - Copying binary files robustly across platforms.
 *   - Preparing workspace structure (mkdir -p equivalents).

 * Notes:
 *   - All functions return 0 on success, non-zero on failure.
 *   - Paths are passed as UTF-8 C strings. On Windows, narrow strings map to ANSI;
 *     for non-ASCII file names prefer UTF-8 narrow build or extend to wide APIs.
 *---------------------------------------------------------------------------*/

#ifndef UENG_FS_H
#define UENG_FS_H

#include "ueng/common.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /* Chapter/outline + workspace helpers used by 'ingest' and 'build'
     These create starter files only if missing, so repeated runs are idempotent. */
  int generate_toc_md(const char *title);
  int generate_frontmatter_md(const char *title, const char *author);
  int generate_acknowledgements_md(const char *author);
  int generate_frontcover_md(const char *title, const char *author, const char *slug);
  /* cover.svg placed in workspace/ for reuse across export/site. */
  int generate_cover_svg(const char *title, const char *author, const char *slug);

  /* Build helpers
     pack_book_draft: concatenates workspace/chapters/*.md => workspace/book-draft.md */
  int pack_book_draft(const char *title, const char *outputs_root, int *out_has_draft);

  /* Theme and site generation
     copy_theme_into_html_dir ensures html/style.css exists and returns "style.css" in out_rel_css.
     write_site_index generates a minimal landing page linking to HTML draft. */
  int copy_theme_into_html_dir(const char *html_dir, char *out_rel_css, size_t outsz);
  int write_site_index(const char *site_dir, const char *title, const char *author,
                       const char *slug, const char *stamp, int has_cover, int has_draft);

#ifdef __cplusplus
}
#endif
#endif /* UENG_FS_H */
