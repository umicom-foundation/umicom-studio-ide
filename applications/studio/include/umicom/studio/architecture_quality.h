/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/architecture_quality.h
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
#ifndef UMICOM_STUDIO_ARCHITECTURE_QUALITY_H
#define UMICOM_STUDIO_ARCHITECTURE_QUALITY_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_architecture_quality_report(const char *root,char *buffer,size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
