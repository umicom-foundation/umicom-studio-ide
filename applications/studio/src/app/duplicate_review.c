/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/duplicate_review.c
 *
 * PURPOSE:
 *   Integrate Framework CodeGuard analysis into Studio security and
 *   consolidation workflows.
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
#include "umicom/studio/duplicate_review.h"
#include "umicom/codeguard/codeguard.h"
UmiStatus umi_studio_duplicate_review(const char *root,size_t *out){UmiCodeGuardResult *r=NULL;UmiCodeGuardConfig c;UmiStatus s;if(root==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=0U;s=umi_codeguard_result_create(256U,&r);if(s==UMI_STATUS_OK){c=umi_codeguard_config_default(root);c.profile.scan_architecture=0;c.profile.scan_duplicates=1;s=umi_codeguard_scan(&c,r);}if(s==UMI_STATUS_OK)*out=umi_codeguard_result_count_category(r,UMI_CODEGUARD_CATEGORY_DUPLICATION);umi_codeguard_result_destroy(r);return s;}
