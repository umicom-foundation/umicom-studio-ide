/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_navigation_history.c
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
#include "umicom/studio/navigation_history.h"
int main(void){
    UmiStudioNavigationHistory h;
    const UmiStudioNavigationLocation *p;
    umi_studio_navigation_history_init(&h);
    assert(umi_studio_navigation_history_push(&h,"a.c",1U,1U)==UMI_STATUS_OK);
    assert(umi_studio_navigation_history_push(&h,"b.c",20U,4U)==UMI_STATUS_OK);
    p=umi_studio_navigation_history_back(&h);
    assert(p!=NULL && strcmp(p->path,"a.c")==0);
    p=umi_studio_navigation_history_forward(&h);
    assert(p!=NULL && strcmp(p->path,"b.c")==0);
    return 0;
}
