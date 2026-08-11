/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/helix_generation_main.c
 *
 * PURPOSE:
 *   Evaluate a reference Helix fitness score and release gate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This native command exercises the same Framework-backed service that future Studio GUI views will call.
 */

#include "umicom/studio/helix_fitness.h"
#include "umicom/studio/helix_generation.h"

#include <stdio.h>

int main(void)
{
    UmiStudioAiPlatform *platform = NULL;
    UmiHelixFitness fitness;
    UmiStatus status = umi_studio_ai_platform_create(&platform);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_helix_fitness(0.95, 0.95, 0.90, 0.90, 0.95, &fitness);
    }
    if (status == UMI_STATUS_OK) {
        umi_studio_ai_platform_helix(platform)->policy.allow_release = 1;
        status = umi_studio_helix_can_promote(platform, &fitness, 4U, 4U, 1, 1);
        printf("Fitness: %.3f passed=%d promotion=%s\n",
               fitness.total, fitness.passed,
               status == UMI_STATUS_OK ? "allowed" : "blocked");
    }
    umi_studio_ai_platform_destroy(platform);
    return status == UMI_STATUS_OK ? 0 : 1;
}
