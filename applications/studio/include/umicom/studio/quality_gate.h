/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/quality_gate.h
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
#ifndef UMICOM_STUDIO_QUALITY_GATE_H
#define UMICOM_STUDIO_QUALITY_GATE_H
#include "umicom/studio/codeguard.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_quality_gate(const char *root,int *out_passed,UmiCodeGuardSummary *out_summary);
#ifdef __cplusplus
}
#endif
#endif
