/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/update_main.c
 *
 * PURPOSE:
 *   Provide a native Studio command for the Framework-backed application-delivery platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command is a thin product-facing wrapper; the reusable delivery logic lives in Umicom Framework.
 */

#include <stdio.h>
#include "umicom/studio/delivery_update.h"
int main(void) {
    UmiUpdatePlan plan;
    if (umi_studio_update_plan(10U, 11U, 1, 1, &plan) != UMI_STATUS_OK) return 1;
    printf("Update: %s\n", umi_update_decision_text(plan.decision));
    printf("Target generation: %llu\n", (unsigned long long)plan.target_generation);
    return 0;
}
