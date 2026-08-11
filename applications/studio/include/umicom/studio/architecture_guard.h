/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/architecture_guard.h
 *
 * PURPOSE:
 *   Expose a simple architecture guard that checks reference products use public Framework contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio is the reference consumer. These adapters call public Framework ABI, SDK and product APIs rather than duplicating those mechanisms inside the IDE.
 */
#ifndef UMICOM_STUDIO_ARCHITECTURE_GUARD_H
#define UMICOM_STUDIO_ARCHITECTURE_GUARD_H
#include "umicom/studio/compatibility_platform.h"
typedef struct UmiStudioArchitectureGuard { size_t profiles_checked; size_t invalid_profiles; size_t incompatible_profiles; int passed; } UmiStudioArchitectureGuard;
UmiStatus umi_studio_architecture_guard_run(UmiStudioArchitectureGuard *out_result);
#endif

