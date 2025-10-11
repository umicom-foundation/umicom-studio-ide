/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/rg_discovery.h
 * PURPOSE: Find ripgrep executable via env or PATH
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_RG_DISCOVERY_H
#define UMICOM_RG_DISCOVERY_H

#include <glib.h>

/* Discovery order:
 *  1) USTUDIO_RG (env var)
 *  2) UMI_RG (env var)
 *  3) PATH lookup: rg(.exe) on current platform
 * Returns newly-allocated string or NULL if not found. */
gchar *umi_rg_find_binary(void);

#endif /* UMICOM_RG_DISCOVERY_H */
