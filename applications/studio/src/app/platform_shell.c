/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/platform_shell.c
 *
 * PURPOSE:
 *   Implement the Studio platform shell as a product-composition owner over reusable Framework platform aggregates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio composes reusable Framework capabilities here. Generic models and
 * engines stay in Framework so the same mechanisms can serve future products.
 */
#include "umicom/studio/platform_shell.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioPlatformShell {UmiStudioServices*services;UmiUiWorkbenchPlatform*workbench;UmiResourceCentre*resources;UmiProductCentre*products;UmiChartWorkspace*charts;UmiDesignerAuthoringSession*designer;UmiFrontendApplication*frontend;UmiStudioDeveloperWorkbench*developer;uint64_t revision;};
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_platform_shell_create(UmiStudioServices*services,UmiStudioPlatformShell**out){UmiStudioPlatformShell*p;UmiStatus s=UMI_STATUS_OK;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;p->services=services;p->revision=1U;if(s==UMI_STATUS_OK)s=umi_ui_workbench_platform_create(&p->workbench);if(s==UMI_STATUS_OK)s=umi_platform_resource_centre_create(&p->resources);if(s==UMI_STATUS_OK)s=umi_product_centre_create(&p->products);if(s==UMI_STATUS_OK)s=umi_chart_workspace_create(&p->charts);if(s==UMI_STATUS_OK)s=umi_designer_authoring_session_create(&p->designer);if(s==UMI_STATUS_OK)s=umi_frontend_application_create(&p->frontend);if(s==UMI_STATUS_OK)s=umi_studio_developer_workbench_create(&p->developer);if(s!=UMI_STATUS_OK){umi_studio_platform_shell_destroy(p);return s;}*out=p;return UMI_STATUS_OK;}
void umi_studio_platform_shell_destroy(UmiStudioPlatformShell*p){if(p==NULL)return;umi_studio_developer_workbench_destroy(p->developer);umi_frontend_application_destroy(p->frontend);umi_designer_authoring_session_destroy(p->designer);umi_chart_workspace_destroy(p->charts);umi_product_centre_destroy(p->products);umi_platform_resource_centre_destroy(p->resources);umi_ui_workbench_platform_destroy(p->workbench);free(p);}
UmiStatus umi_studio_platform_shell_snapshot(UmiStudioPlatformShell*p,UmiStudioPlatformShellSnapshot*o){if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;copy_text(o->area_id,sizeof(o->area_id),"studio.platform-shell");copy_text(o->view_type,sizeof(o->view_type),"studio.platform-shell");copy_text(o->title,sizeof(o->title),"Platform Shell");copy_text(o->summary,sizeof(o->summary),"Unified Studio shell, developer services, resources, products, charts, designer and multi-frontend platform.");o->revision=p->revision;o->available=1;if(umi_ui_workbench_platform_snapshot(p->workbench,&o->workbench)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_platform_resource_centre_snapshot(p->resources,&o->resources)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_product_centre_snapshot(p->products,&o->products)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_chart_workspace_snapshot(p->charts,&o->charts)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_designer_authoring_session_snapshot(p->designer,&o->designer)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_frontend_application_snapshot(p->frontend,&o->frontend)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;if(umi_studio_developer_workbench_snapshot(p->developer,&o->developer)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;o->item_count=o->workbench.list_items+o->resources.recent_count+o->products.marketplace_count+o->charts.panes_count+o->designer.templates_count+o->frontend.widgets_count+o->developer.item_count;return UMI_STATUS_OK;}
#define A(fn,type,field) type *fn(UmiStudioPlatformShell*p){return p!=NULL?p->field:NULL;}
A(umi_studio_platform_shell_workbench,UmiUiWorkbenchPlatform,workbench) A(umi_studio_platform_shell_resources,UmiResourceCentre,resources) A(umi_studio_platform_shell_products,UmiProductCentre,products) A(umi_studio_platform_shell_charts,UmiChartWorkspace,charts) A(umi_studio_platform_shell_designer,UmiDesignerAuthoringSession,designer) A(umi_studio_platform_shell_frontend,UmiFrontendApplication,frontend) A(umi_studio_platform_shell_developer,UmiStudioDeveloperWorkbench,developer)
#undef A

UmiStatus umi_studio_platform_shell_activate_project(
    UmiStudioPlatformShell *shell,
    const UmiProjectWorkspaceSelectionRequest *request,
    UmiProjectWorkspaceSelectionSnapshot *out_selection)
{
    UmiStatus status;
    if(shell==NULL||request==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_studio_developer_workbench_activate_project(
        shell->developer,request,out_selection);
    if(status==UMI_STATUS_OK)shell->revision+=1U;
    return status;
}

UmiStatus umi_studio_platform_shell_validate_project(
    UmiStudioPlatformShell *shell,
    UmiProjectWorkspaceValidationReport *out_report)
{
    if(shell==NULL||out_report==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    return umi_studio_developer_workbench_validate_project(
        shell->developer,out_report);
}

UmiStatus umi_studio_platform_shell_prepare_project_workflow(
    UmiStudioPlatformShell *shell,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow)
{
    UmiStatus status;
    if(shell==NULL||request==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_studio_developer_workbench_prepare_project_workflow(
        shell->developer,request,out_workflow);
    if(status==UMI_STATUS_OK)shell->revision+=1U;
    return status;
}

UmiStatus umi_studio_platform_shell_import_project(
    UmiStudioPlatformShell *shell,
    const UmiDeveloperProjectBootstrapRequest *request,
    UmiDeveloperProjectBootstrapSnapshot *out_snapshot)
{
    UmiStatus status;
    if (shell == NULL || request == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_developer_workbench_import_project(
        shell->developer, request, out_snapshot);
    if (status == UMI_STATUS_OK) shell->revision += 1U;
    return status;
}
