/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_agent_operations.h
 * PURPOSE: Build canonical filesystem, build, test and review action descriptors.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Canonical constructors keep action identifiers and retry
 * limits consistent across command palette, tools and workbench views. */
#ifndef UMICOM_STUDIO_HELIX_AGENT_OPERATIONS_H
#define UMICOM_STUDIO_HELIX_AGENT_OPERATIONS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_agent_filesystem_action(
    const char *target, UmiHelixAction *out_action);
UmiStatus umi_studio_helix_agent_build_action(
    const char *target, UmiHelixAction *out_action);
UmiStatus umi_studio_helix_agent_test_action(
    const char *target, UmiHelixAction *out_action);
UmiStatus umi_studio_helix_agent_review_action(
    const char *target, UmiHelixAction *out_action);
UmiStatus umi_studio_helix_agent_source_control_action(
    const char *target, UmiHelixAction *out_action);

#ifdef __cplusplus
}
#endif

#endif
