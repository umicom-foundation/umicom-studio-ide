/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/duplicate_review.h
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
#ifndef UMICOM_STUDIO_DUPLICATE_REVIEW_H
#define UMICOM_STUDIO_DUPLICATE_REVIEW_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_duplicate_review(const char *root,size_t *out_duplicates);
#ifdef __cplusplus
}
#endif
#endif
