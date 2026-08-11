/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/tests/test_runtime_filter.c
 *
 * PURPOSE:
 *   Exercise one Studio Application Hub runtime behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/studio/runtime_catalogue.h"
#include "umicom/studio/runtime_filter.h"
int main(void){
    UmiStudioRuntimeManager m;
    UmiStudioRuntimeFilter f;
    const UmiStudioRuntimeEntry *e;
    umi_studio_runtime_manager_init(&m);
    umi_studio_runtime_filter_init(&f);
    assert(umi_studio_runtime_catalogue_populate(&m)==UMI_STATUS_OK);
    e=umi_studio_runtime_manager_find_const(&m,"umicom.trader");
    assert(e!=NULL);
    (void)snprintf(f.query,sizeof(f.query),"%s","risk");
    assert(umi_studio_runtime_filter_match(&f,e,UMI_INTEGRATION_APP_DISCOVERED));
    f.category=UMI_STUDIO_RUNTIME_CATEGORY_AI;
    assert(!umi_studio_runtime_filter_match(&f,e,UMI_INTEGRATION_APP_DISCOVERED));
    return 0;
}
