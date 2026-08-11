/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_delivery_update.c
 *
 * PURPOSE:
 *   Verify one Studio integration path over the Framework delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The test keeps product integration small so failures are easy for a new developer to locate.
 */

#include <assert.h>
#include "umicom/studio/delivery_update.h"
int main(void) {
    UmiUpdatePlan update;
    UmiRollbackPlan rollback;
    assert(umi_studio_update_plan(1U, 2U, 1, 1, &update) == UMI_STATUS_OK);
    assert(update.decision == UMI_UPDATE_AVAILABLE);
    assert(umi_studio_rollback_plan(2U, 1U, "test", &rollback) == UMI_STATUS_OK);
    assert(umi_rollback_plan_valid(&rollback));
    return 0;
}
