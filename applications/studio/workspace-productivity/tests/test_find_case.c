/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_find_case.c
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
#include "umicom/studio/find_replace.h"
int main(void){
    UmiStudioFindReplace f;
    size_t count=0U;
    umi_studio_find_replace_init(&f);
    assert(umi_studio_find_replace_set(&f,"studio","")==UMI_STATUS_OK);
    assert(umi_studio_find_count("Studio studio",&f,&count)==UMI_STATUS_OK);
    assert(count==2U);
    f.case_sensitive=true;
    assert(umi_studio_find_count("Studio studio",&f,&count)==UMI_STATUS_OK);
    assert(count==1U);
    return 0;
}
