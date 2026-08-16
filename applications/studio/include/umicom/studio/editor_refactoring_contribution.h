/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/editor_refactoring_contribution.h
 *
 * PURPOSE:
 *   Define Studio placement for Framework-owned quick-fix and refactoring
 *   commands and views without duplicating any provider or edit logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_H
#define UMICOM_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/editor/refactoring_command.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_API_VERSION 1U

typedef struct UmiStudioEditorRefactoringCommandContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *framework_command_id;
    const char *menu_id;
    const char *menu_group;
    int32_t order;
    int show_in_command_centre;
} UmiStudioEditorRefactoringCommandContribution;

typedef struct UmiStudioEditorRefactoringViewContribution {
    uint32_t struct_size;
    uint32_t api_version;
    const char *view_id;
    const char *title;
    const char *framework_contract;
    const char *default_region;
    int32_t order;
    int closable;
    int movable;
} UmiStudioEditorRefactoringViewContribution;

size_t umi_studio_editor_refactoring_command_contribution_count(void);
const UmiStudioEditorRefactoringCommandContribution *
umi_studio_editor_refactoring_command_contribution_at(size_t index);
const UmiStudioEditorRefactoringCommandContribution *
umi_studio_editor_refactoring_command_contribution_find(
    const char *framework_command_id);
size_t umi_studio_editor_refactoring_view_contribution_count(void);
const UmiStudioEditorRefactoringViewContribution *
umi_studio_editor_refactoring_view_contribution_at(size_t index);
const UmiStudioEditorRefactoringViewContribution *
umi_studio_editor_refactoring_view_contribution_find(const char *view_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_EDITOR_REFACTORING_CONTRIBUTION_H */
