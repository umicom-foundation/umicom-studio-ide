/*-----------------------------------------------------------------------------
 * Umicom Studio IDE - Visual Builder v2 command boundary
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_commands.h"
#include "umicom/studio/visual_builder_clipboard.h"
#include "umicom/studio/visual_builder_preview.h"
#include "umicom/studio/visual_builder_source.h"
#include <string.h>
void umi_studio_visual_builder_command_init(UmiStudioVisualBuilderCommand *command,UmiStudioVisualBuilderCommandKind kind){if(command==NULL)return;(void)memset(command,0,sizeof(*command));command->kind=kind;}
UmiStatus umi_studio_visual_builder_command_execute(UmiStudioVisualBuilderCentre *centre,const UmiStudioVisualBuilderCommand *command)
{
    UmiDesignerBuilderSession *session=umi_studio_visual_builder_centre_session(centre); UmiDesignerTransactionHistory *transactions;
    if(session==NULL||command==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    transactions=umi_designer_builder_session_transactions(session);
    switch(command->kind){case UMI_STUDIO_VISUAL_BUILDER_ADD:return umi_designer_builder_session_add_component(session,command->node_id,command->component_type,command->parent_id);case UMI_STUDIO_VISUAL_BUILDER_UNDO:return umi_designer_transaction_history_undo_count(transactions)>0U?umi_designer_transaction_history_undo(transactions):umi_designer_history_undo(umi_designer_builder_session_history(session));case UMI_STUDIO_VISUAL_BUILDER_REDO:return umi_designer_transaction_history_redo_count(transactions)>0U?umi_designer_transaction_history_redo(transactions):umi_designer_history_redo(umi_designer_builder_session_history(session));case UMI_STUDIO_VISUAL_BUILDER_COPY:return umi_studio_visual_builder_copy(centre);case UMI_STUDIO_VISUAL_BUILDER_PASTE:return umi_studio_visual_builder_paste(centre,command->parent_id,command->id_prefix);case UMI_STUDIO_VISUAL_BUILDER_REFRESH_PREVIEW:return umi_studio_visual_builder_preview_refresh(centre);case UMI_STUDIO_VISUAL_BUILDER_GENERATE_SOURCE:return umi_studio_visual_builder_source_refresh(centre);default:return UMI_STATUS_INVALID_ARGUMENT;}
}
