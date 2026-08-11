/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_recent_workspaces.c
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
#include <string.h>
#include "umicom/studio/recent_workspaces.h"
int main(void){
    UmiStudioRecentWorkspaceList r;
    umi_studio_recent_workspaces_init(&r);
    assert(umi_studio_recent_workspaces_touch(&r,"A","Alpha")==UMI_STATUS_OK);
    assert(umi_studio_recent_workspaces_touch(&r,"B","Beta")==UMI_STATUS_OK);
    assert(strcmp(umi_studio_recent_workspaces_at(&r,0)->path,"B")==0);
    assert(umi_studio_recent_workspaces_touch(&r,"A","Alpha")==UMI_STATUS_OK);
    assert(strcmp(umi_studio_recent_workspaces_at(&r,0)->path,"A")==0);
    return 0;
}
