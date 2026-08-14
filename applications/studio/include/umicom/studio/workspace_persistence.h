/* Umicom Studio IDE | Workspace persistence v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_WORKSPACE_PERSISTENCE_H
#define UMICOM_STUDIO_WORKSPACE_PERSISTENCE_H
#include "umicom/studio/professional_workspace.h"
UmiStatus umi_studio_workspace_persistence_export_active(UmiStudioProfessionalWorkspace *workspace,uint64_t saved_at_ns,char *out_text,size_t capacity);
UmiStatus umi_studio_workspace_persistence_import(UmiStudioProfessionalWorkspace *workspace,const char *text,bool activate);
#endif
