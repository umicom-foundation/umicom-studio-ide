/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_tab_policy.c
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
#include "umicom/studio/tab_policy.h"
int main(void){
    UmiStudioWorkspaceDocument d;
    char label[256];
    umi_studio_workspace_document_init(&d);
    assert(umi_studio_workspace_document_set_path(&d,"src/main.c")==UMI_STATUS_OK);
    d.pinned=true;d.dirty=true;
    assert(umi_studio_tab_close_decision(&d)==UMI_STUDIO_TAB_CLOSE_CONFIRM_DIRTY);
    assert(umi_studio_tab_label(&d,label,sizeof(label))==UMI_STATUS_OK);
    assert(strstr(label,"[P]")!=NULL);
    assert(strstr(label,"*")!=NULL);
    return 0;
}
