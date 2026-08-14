/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 command boundary
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Keyboard, menu and toolbar actions call the same command contract. */
#ifndef UMICOM_STUDIO_VISUAL_BUILDER_COMMANDS_H
#define UMICOM_STUDIO_VISUAL_BUILDER_COMMANDS_H
#include "umicom/studio/visual_builder_centre.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiStudioVisualBuilderCommandKind {UMI_STUDIO_VISUAL_BUILDER_ADD=1,UMI_STUDIO_VISUAL_BUILDER_UNDO=2,UMI_STUDIO_VISUAL_BUILDER_REDO=3,UMI_STUDIO_VISUAL_BUILDER_COPY=4,UMI_STUDIO_VISUAL_BUILDER_PASTE=5,UMI_STUDIO_VISUAL_BUILDER_REFRESH_PREVIEW=6,UMI_STUDIO_VISUAL_BUILDER_GENERATE_SOURCE=7} UmiStudioVisualBuilderCommandKind;
typedef struct UmiStudioVisualBuilderCommand {UmiStudioVisualBuilderCommandKind kind;char node_id[UMI_DECL_ID_CAPACITY];char component_type[UMI_DECL_ID_CAPACITY];char parent_id[UMI_DECL_ID_CAPACITY];char id_prefix[UMI_DECL_ID_CAPACITY];} UmiStudioVisualBuilderCommand;
void umi_studio_visual_builder_command_init(UmiStudioVisualBuilderCommand *command,UmiStudioVisualBuilderCommandKind kind);
UmiStatus umi_studio_visual_builder_command_execute(UmiStudioVisualBuilderCentre *centre,const UmiStudioVisualBuilderCommand *command);
#ifdef __cplusplus
}
#endif
#endif
