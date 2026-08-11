/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/consolidation.h
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
#ifndef UMICOM_STUDIO_CONSOLIDATION_H
#define UMICOM_STUDIO_CONSOLIDATION_H
#include <stddef.h>
#include "umicom/studio/codeguard.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioConsolidationReport { size_t architecture_findings; size_t duplicates; size_t high_risk; int ready; } UmiStudioConsolidationReport;
UmiStatus umi_studio_consolidation_check(const char *root,UmiStudioConsolidationReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
