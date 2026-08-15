/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/workspace_profiles.h
 *
 * PURPOSE:
 *   Declare Studio's named profiles for the live Framework workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_STUDIO_WORKSPACE_PROFILES_H
#define UMICOM_STUDIO_WORKSPACE_PROFILES_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_WORKSPACE_PROFILE_DEVELOP "develop"
#define UMI_STUDIO_WORKSPACE_PROFILE_FOCUS "focus"
#define UMI_STUDIO_WORKSPACE_PROFILE_DEBUG "debug"
#define UMI_STUDIO_WORKSPACE_PROFILE_SOURCE_CONTROL "source-control"
#define UMI_STUDIO_WORKSPACE_PROFILE_TESTING "testing"
#define UMI_STUDIO_WORKSPACE_PROFILE_BUILD "build"
#define UMI_STUDIO_WORKSPACE_PROFILE_REVIEW "review"
#define UMI_STUDIO_WORKSPACE_PROFILE_OPERATIONS "operations"
#define UMI_STUDIO_WORKSPACE_PROFILE_TRADING "trading"
#define UMI_STUDIO_WORKSPACE_PROFILE_COUNT 9U

UmiStatus umi_studio_workspace_profiles_register(UmiUiWorkbench *workbench);

#ifdef __cplusplus
}
#endif

#endif
