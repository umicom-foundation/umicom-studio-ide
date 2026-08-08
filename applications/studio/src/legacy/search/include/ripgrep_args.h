/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/search/include/ripgrep_args.h
 * PURPOSE: Helpers to construct argv arrays for ripgrep invocations
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#pragma once
#include <glib.h>

/*---------------------------------------------------------------------------
 * umi_rg_args_make_simple:
 *   Build a NULL-terminated argv suitable for glib's g_spawn* APIs.
 *   The returned array and all contained strings are newly allocated and
 *   must be released with umi_rg_args_free().
 *---------------------------------------------------------------------------*/
char **umi_rg_args_make_simple(const char *pattern, const char *path);

/*---------------------------------------------------------------------------
 * umi_rg_args_free:
 *   Frees an argv vector previously created by umi_rg_args_make_simple().
 *   Safe with NULL.
 *---------------------------------------------------------------------------*/
void umi_rg_args_free(char **argvv);
/*---------------------------------------------------------------------------*/