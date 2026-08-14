/* Umicom Studio IDE | Workspace persistence v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_persistence.h"
#include <string.h>
UmiStatus umi_studio_workspace_persistence_export_active(UmiStudioProfessionalWorkspace *workspace,uint64_t saved_at_ns,char *out_text,size_t capacity)
{
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    UmiUiWorkspaceLayout *layout;
    UmiUiLayoutPersistenceRecord record;
    if (model == NULL || out_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_ui_workspace_customisation_active(model);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    (void)memset(&record,0,sizeof(record));
    record.schema_version = 2U; record.saved_at_ns = saved_at_ns; record.layout = *layout;
    return umi_ui_layout_persistence_encode(&record,out_text,capacity);
}
UmiStatus umi_studio_workspace_persistence_import(UmiStudioProfessionalWorkspace *workspace,const char *text,bool activate)
{
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    UmiUiLayoutPersistenceRecord record;
    UmiStatus status;
    if (model == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_layout_persistence_decode(text,&record);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_customisation_add_layout(model,&record.layout);
    if (status != UMI_STATUS_OK) return status;
    return activate ? umi_ui_workspace_customisation_activate(model,record.layout.layout_id) : UMI_STATUS_OK;
}
