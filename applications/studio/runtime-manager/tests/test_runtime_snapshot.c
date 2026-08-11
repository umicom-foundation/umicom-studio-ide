/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/tests/test_runtime_snapshot.c
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
#include "umicom/studio/runtime_catalogue.h"
#include "umicom/studio/runtime_snapshot.h"
int main(void){
    UmiStudioRuntimeManager m;
    UmiStudioRuntimeSnapshot s;
    umi_studio_runtime_manager_init(&m);
    assert(umi_studio_runtime_catalogue_populate(&m)==UMI_STATUS_OK);
    assert(umi_studio_runtime_snapshot_build(&m,&s)==UMI_STATUS_OK);
    assert(s.total==m.count);
    assert(s.running>=1U);
    assert(s.favourites>=1U);
    return 0;
}
