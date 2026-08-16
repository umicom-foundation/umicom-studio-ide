/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/source_navigation_contribution.h
 *
 * PURPOSE:
 *   Define Studio menu placement for Framework-owned navigation commands
 *   without copying their identities, labels, keybindings or behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_H
#define UMICOM_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/navigation_command.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_API_VERSION 1U

typedef struct UmiStudioSourceNavigationContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *framework_command_id;
    const char *menu_id;
    const char *menu_group;
    int32_t order;
    int show_in_command_centre;
} UmiStudioSourceNavigationContribution;

size_t umi_studio_source_navigation_contribution_count(void);
const UmiStudioSourceNavigationContribution *
umi_studio_source_navigation_contribution_at(size_t index);
const UmiStudioSourceNavigationContribution *
umi_studio_source_navigation_contribution_find(
    const char *framework_command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_SOURCE_NAVIGATION_CONTRIBUTION_H */
