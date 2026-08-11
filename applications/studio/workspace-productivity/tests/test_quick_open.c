/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_quick_open.c
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
#include "umicom/studio/quick_open.h"
int main(void){
    UmiStudioQuickOpenCandidate c[]={
        {"window.c","src/gui/window.c",UMI_STUDIO_QUICK_OPEN_FILE,0},
        {"workspace.c","src/core/workspace.c",UMI_STUDIO_QUICK_OPEN_FILE,0},
        {"main.c","src/gtk/main.c",UMI_STUDIO_QUICK_OPEN_FILE,0}
    };
    UmiStudioQuickOpenResults r;
    assert(umi_studio_quick_open_search("win",c,3U,&r)==UMI_STATUS_OK);
    assert(r.count>=1U);
    assert(strcmp(r.items[0].label,"window.c")==0);
    return 0;
}
