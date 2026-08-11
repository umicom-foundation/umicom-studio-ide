/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_session_state.c
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
#include "umicom/studio/session_state.h"
int main(void){
    UmiStudioSessionState a,b;
    char text[UMI_STUDIO_SESSION_TEXT_CAPACITY];
    umi_studio_session_state_init(&a);
    assert(umi_studio_session_state_set_workspace(&a,"C:/Dev/Studio")==UMI_STATUS_OK);
    assert(umi_studio_document_set_open(&a.documents,"main.c","C",false)==UMI_STATUS_OK);
    assert(umi_studio_document_set_pin(&a.documents,"main.c",true)==UMI_STATUS_OK);
    assert(umi_studio_session_state_encode(&a,text,sizeof(text))==UMI_STATUS_OK);
    assert(umi_studio_session_state_decode(text,&b)==UMI_STATUS_OK);
    assert(strcmp(b.workspace_path,"C:/Dev/Studio")==0);
    assert(b.documents.count==1U);
    assert(b.documents.documents[0].pinned);
    return 0;
}
