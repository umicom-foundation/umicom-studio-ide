/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/include/status.h
 *
 * PURPOSE:
 *   Compatibility shim because project_manager.h includes "status.h", but
 *   the actual public status utilities live in util/log/include/status_util.h.
 *   This header simply forwards to the canonical one.
 *
 * DESIGN:
 *   - Do not re-declare anything here; just include the real header.
 *   - Keeps modules loosely coupled via PUBLIC include dirs.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_STATUS_SHIM_H
#define UMI_STATUS_SHIM_H

/* Forward to canonical status utilities (public include path). */
#include <status_util.h>

#endif /* UMI_STATUS_SHIM_H */
