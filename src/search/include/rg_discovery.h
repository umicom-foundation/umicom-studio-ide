/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/include/rg_discovery.h
 * PURPOSE: Discover ripgrep binary and capture its version
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once

#include <glib.h>

/*-----------------------------------------------------------------------------
 * UmiRgProbe:
 *   Small POD used to report the discovered ripgrep path and version string.
 *   - path: absolute or search-resolved path to the 'rg' executable
 *   - version: human-readable version line as printed by 'rg --version'
 *---------------------------------------------------------------------------*/
typedef struct UmiRgProbe {
  gchar *path;     /* g_malloc'd string; caller must g_free(). */
  gchar *version;  /* g_malloc'd string; caller must g_free(). */
} UmiRgProbe;

/*-----------------------------------------------------------------------------
 * umi_rg_discover:
 *   Try to locate a usable ripgrep (rg) executable in PATH.
 *   Returns: newly-allocated UmiRgProbe on success, or NULL if not found.
 *   Ownership: caller owns the returned struct and must call umi_rg_probe_free().
 *---------------------------------------------------------------------------*/
UmiRgProbe *umi_rg_discover(void);

/*-----------------------------------------------------------------------------
 * umi_rg_probe_free:
 *   Frees a UmiRgProbe produced by umi_rg_discover(). Safe with NULL.
 *---------------------------------------------------------------------------*/
void umi_rg_probe_free(UmiRgProbe *p);
/*---------------------------------------------------------------------------*/