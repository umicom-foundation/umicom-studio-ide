/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_workspace_summary.c
 *
 * PURPOSE:
 *   Exercise one workspace/editor productivity behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of Studio workspace/editor productivity. The implementation
 * uses bounded C23 data structures and explicit ownership so the behaviour is
 * easy to inspect, test and later connect to the main Studio workbench.
 */

#include <assert.h>
#include "umicom/studio/workspace_summary.h"
int main(void){
    UmiStudioDocumentSet d;
    UmiStudioRecentWorkspaceList r;
    UmiStudioWorkspaceSummary s;
    umi_studio_document_set_init(&d);
    umi_studio_recent_workspaces_init(&r);
    assert(umi_studio_document_set_open(&d,"a.c","C",false)==UMI_STATUS_OK);
    assert(umi_studio_document_set_mark_dirty(&d,"a.c",true)==UMI_STATUS_OK);
    assert(umi_studio_recent_workspaces_touch(&r,"A","A")==UMI_STATUS_OK);
    assert(umi_studio_workspace_summary_build(&d,&r,&s)==UMI_STATUS_OK);
    assert(s.open_documents==1U);
    assert(s.dirty_documents==1U);
    assert(s.recent_workspaces==1U);
    return 0;
}
