/* Umicom Studio IDE | Workspace persistence v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_PERSISTENCE_V2_H
#define UMICOM_STUDIO_WORKSPACE_PERSISTENCE_V2_H
#include "umicom/studio/professional_workspace_v2.h"
UmiStatus umi_studio_workspace_persistence_v2_export_active(UmiStudioProfessionalWorkspaceV2 *workspace,uint64_t saved_at_ns,char *out_text,size_t capacity);
UmiStatus umi_studio_workspace_persistence_v2_import(UmiStudioProfessionalWorkspaceV2 *workspace,const char *text,bool activate);
#endif
