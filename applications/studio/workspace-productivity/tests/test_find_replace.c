/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/workspace-productivity/tests/test_find_replace.c
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
#include "umicom/studio/find_replace.h"
int main(void){
    UmiStudioFindReplace f;
    char output[128];
    size_t count=0U, replaced=0U;
    umi_studio_find_replace_init(&f);
    assert(umi_studio_find_replace_set(&f,"Studio","IDE")==UMI_STATUS_OK);
    assert(umi_studio_find_count("Studio StudioX Studio",&f,&count)==UMI_STATUS_OK);
    assert(count==3U);
    f.whole_word=true;
    assert(umi_studio_find_count("Studio StudioX Studio",&f,&count)==UMI_STATUS_OK);
    assert(count==2U);
    assert(umi_studio_replace_all("Studio Studio",&f,output,sizeof(output),&replaced)==UMI_STATUS_OK);
    assert(replaced==2U);
    assert(strcmp(output,"IDE IDE")==0);
    return 0;
}
