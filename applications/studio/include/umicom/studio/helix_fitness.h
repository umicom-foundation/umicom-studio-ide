/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_fitness.h
 *
 * PURPOSE:
 *   Calculate the standard Studio Helix fitness score for one candidate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio uses the Framework fitness calculation unchanged so candidate acceptance is consistent across CLI, GUI and future Helix agents.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_FITNESS_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_FITNESS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_fitness(double correctness,
                                   double security,
                                   double performance,
                                   double maintainability,
                                   double compatibility,
                                   UmiHelixFitness *fitness);

#ifdef __cplusplus
}
#endif

#endif
