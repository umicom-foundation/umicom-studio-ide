/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/plugins/transpile/include/transpile.h
 * PURPOSE:
 *   Public “Transpile” demo entry points.
 *
 * DESIGN:
 *   - Header keeps dependencies light; only GLib and our i18n header.
 *   - UI interactions are optional; callers can ignore return values and
 *     handle messages themselves if desired.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-01
 * License: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMI_TRANSPILE_H
#define UMI_TRANSPILE_H

#include <glib.h>
#include "i18n_translate.h"

G_BEGIN_DECLS

/* Simple demo that translates a fixed sample and logs/statuses the result. */
void umi_transpile_demo(void);

G_END_DECLS

#endif /* UMI_TRANSPILE_H */
/*--- end of file ---*/