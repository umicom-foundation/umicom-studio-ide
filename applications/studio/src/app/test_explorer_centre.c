/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/test_explorer_centre.c
 *
 * PURPOSE:
 *   Implement the Test Explorer Centre composition.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/test_explorer_centre.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioTestExplorerCentre {
    UmiTestPlatformService *service;
    UmiTestWorkspace *workspace;
    UmiStudioTestService *test_service;
    uint64_t revision;
    int owns_service;
    int owns_workspace;
};
static void copy_text(char*d,size_t c,const char*s){size_t n;if(d==NULL||c==0U)return;if(s==NULL)s="";n=strlen(s);if(n>=c)n=c-1U;if(n>0U)memcpy(d,s,n);d[n]='\0';}
UmiStatus umi_studio_test_explorer_centre_create(UmiStudioTestExplorerCentre **out){UmiStudioTestExplorerCentre*p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_test_platform_service_create(&p->service);if(s==UMI_STATUS_OK){p->owns_service=1;s=umi_test_workspace_create(p->service,&p->workspace);}if(s!=UMI_STATUS_OK){umi_studio_test_explorer_centre_destroy(p);return s;}p->owns_workspace=1;p->revision=1U;*out=p;return UMI_STATUS_OK;}
UmiStatus umi_studio_test_explorer_centre_create_bound(UmiStudioTestService *tests,UmiStudioTestExplorerCentre **out){UmiStudioTestExplorerCentre*p;if(tests==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;p->test_service=tests;p->service=umi_studio_test_service_platform(tests);p->workspace=umi_studio_test_service_workspace(tests);if(p->service==NULL||p->workspace==NULL){free(p);return UMI_STATUS_INVALID_STATE;}p->revision=1U;*out=p;return UMI_STATUS_OK;}
void umi_studio_test_explorer_centre_destroy(UmiStudioTestExplorerCentre*p){if(p==NULL)return;if(p->owns_workspace)umi_test_workspace_destroy(p->workspace);if(p->owns_service)umi_test_platform_service_destroy(p->service);free(p);}
UmiStatus umi_studio_test_explorer_centre_snapshot(UmiStudioTestExplorerCentre*p,UmiStudioTestExplorerCentreSnapshot*o){
    UmiStatus s;
    if(p==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(o,0,sizeof(*o));
    o->struct_size=(uint32_t)sizeof(*o);
    o->api_version=3U;
    copy_text(o->area_id,sizeof(o->area_id),"studio.test-explorer-centre");
    copy_text(o->title,sizeof(o->title),"Test Explorer Centre");
    copy_text(o->summary,sizeof(o->summary),"CTest metadata, professional selection and filters, selected/all/repeat/rerun operations, retained results, output, coverage and run history.");
    s=umi_test_platform_service_snapshot(p->service,&o->service);
    if(s!=UMI_STATUS_OK)return s;
    s=umi_test_workspace_snapshot(p->workspace,&o->workspace);
    if(s!=UMI_STATUS_OK)return s;
    if(p->test_service!=NULL){
        UmiTestPlatformHierarchyNode *nodes=(UmiTestPlatformHierarchyNode *)calloc(
            UMI_TEST_PLATFORM_SELECTION_CAPACITY,sizeof(*nodes));
        if(nodes==NULL)return UMI_STATUS_OUT_OF_MEMORY;
        s=umi_studio_test_service_explorer_state(p->test_service,&o->explorer);
        if(s==UMI_STATUS_OK)s=umi_studio_test_service_hierarchy(
            p->test_service,nodes,UMI_TEST_PLATFORM_SELECTION_CAPACITY,
            &o->hierarchy_count);
        free(nodes);
        if(s!=UMI_STATUS_OK)return s;
    }
    o->selected_count=o->workspace.visible_item_count;
    o->operation_running=o->service.operation_running;
    o->revision=p->revision>o->workspace.revision?p->revision:o->workspace.revision;
    o->available=1;
    return UMI_STATUS_OK;
}
UmiTestPlatformService *umi_studio_test_explorer_centre_service(UmiStudioTestExplorerCentre*p){return p!=NULL?p->service:NULL;}
UmiTestWorkspace *umi_studio_test_explorer_centre_workspace(UmiStudioTestExplorerCentre*p){return p!=NULL?p->workspace:NULL;}
UmiStatus umi_studio_test_explorer_centre_set_workspace(UmiStudioTestExplorerCentre*p,const char*root,const char*project,uint64_t revision){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_set_workspace(p->test_service,root,project,revision);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
UmiStatus umi_studio_test_explorer_centre_set_filter(UmiStudioTestExplorerCentre*p,const char*text,const char*label,int outcome,int disabled){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_set_filter(p->test_service,text,label,outcome,disabled);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
UmiStatus umi_studio_test_explorer_centre_hierarchy(UmiStudioTestExplorerCentre*p,UmiTestPlatformHierarchyNode*nodes,size_t capacity,size_t*out_count){if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;return umi_studio_test_service_hierarchy(p->test_service,nodes,capacity,out_count);}
UmiStatus umi_studio_test_explorer_centre_plan_all(UmiStudioTestExplorerCentre*p,uint32_t repeat,int stop,UmiTestPlatformOperationPlan*plan){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_plan_all(p->test_service,repeat,stop,plan);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
UmiStatus umi_studio_test_explorer_centre_plan_failed(UmiStudioTestExplorerCentre*p,UmiTestPlatformOperationPlan*plan){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_plan_failed(p->test_service,plan);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
UmiStatus umi_studio_test_explorer_centre_begin(UmiStudioTestExplorerCentre*p,const UmiTestPlatformOperationPlan*plan){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_begin(p->test_service,plan);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
UmiStatus umi_studio_test_explorer_centre_stop(UmiStudioTestExplorerCentre*p){UmiStatus s;if(p==NULL||p->test_service==NULL)return UMI_STATUS_INVALID_STATE;s=umi_studio_test_service_stop(p->test_service);if(s==UMI_STATUS_OK)p->revision+=1U;return s;}
void umi_studio_test_explorer_centre_finish(UmiStudioTestExplorerCentre*p){if(p==NULL||p->test_service==NULL)return;umi_studio_test_service_finish(p->test_service);p->revision+=1U;}
