/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/editor_command_contribution.h
 *
 * PURPOSE:
 *   Define Studio menu placement for Framework-owned editing commands without
 *   copying their labels, keybindings or execution logic into the product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_EDITOR_COMMAND_CONTRIBUTION_H
#define UMICOM_STUDIO_EDITOR_COMMAND_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/edit_command.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_EDITOR_COMMAND_CONTRIBUTION_API_VERSION 1U

typedef struct UmiStudioEditorCommandContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *framework_command_id;
    const char *menu_id;
    const char *menu_group;
    int32_t order;
    int show_in_command_centre;
} UmiStudioEditorCommandContribution;

size_t umi_studio_editor_command_contribution_count(void);
const UmiStudioEditorCommandContribution *
umi_studio_editor_command_contribution_at(size_t index);
const UmiStudioEditorCommandContribution *
umi_studio_editor_command_contribution_find(const char *framework_command_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_EDITOR_COMMAND_CONTRIBUTION_H */
