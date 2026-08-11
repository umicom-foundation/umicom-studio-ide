/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/helix_fitness.c
 *
 * PURPOSE:
 *   Calculate Studio candidate fitness using Framework policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The wrapper fixes the current Studio acceptance threshold while leaving the reusable weighting algorithm in Framework.
 */

#include "umicom/studio/helix_fitness.h"

UmiStatus umi_studio_helix_fitness(double correctness,
                                   double security,
                                   double performance,
                                   double maintainability,
                                   double compatibility,
                                   UmiHelixFitness *fitness)
{
    return umi_helix_fitness_calculate(fitness,
                                       correctness,
                                       security,
                                       performance,
                                       maintainability,
                                       compatibility,
                                       0.85);
}
