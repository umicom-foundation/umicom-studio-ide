/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/rollback_main.c
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
    UmiRollbackPlan plan;
    if (umi_studio_rollback_plan(5U, 4U, "operator requested", &plan) != UMI_STATUS_OK) return 1;
    printf("Rollback: %llu -> %llu (%s)\n",
           (unsigned long long)plan.current_generation,
           (unsigned long long)plan.target_generation,
           umi_rollback_plan_valid(&plan) ? "approved" : "blocked");
    return 0;
}
