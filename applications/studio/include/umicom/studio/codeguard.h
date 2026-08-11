/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/codeguard.h
 *
 * PURPOSE:
 *   Expose Studio integration for CodeGuard security, duplication and
 *   architecture quality analysis.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_STUDIO_CODEGUARD_H
#define UMICOM_STUDIO_CODEGUARD_H
#include "umicom/codeguard/codeguard.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioCodeGuard UmiStudioCodeGuard;
UmiStatus umi_studio_codeguard_create(const char *root,UmiStudioCodeGuard **out_guard);
void umi_studio_codeguard_destroy(UmiStudioCodeGuard *guard);
UmiStatus umi_studio_codeguard_scan(UmiStudioCodeGuard *guard,const UmiCodeGuardProfile *profile);
const UmiCodeGuardResult *umi_studio_codeguard_result(const UmiStudioCodeGuard *guard);
UmiCodeGuardSummary umi_studio_codeguard_summary(const UmiStudioCodeGuard *guard);
#ifdef __cplusplus
}
#endif
#endif
