/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_generation.h
 *
 * PURPOSE:
 *   Evaluate whether a Studio Helix candidate may be promoted to a generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Promotion is evaluated through Framework release gates; the Studio UI cannot promote merely because a button was clicked.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_GENERATION_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_GENERATION_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_can_promote(UmiStudioAiPlatform *platform,
                                       const UmiHelixFitness *fitness,
                                       uint32_t required_checks,
                                       uint32_t passed_checks,
                                       int human_approved,
                                       int rollback_ready);

#ifdef __cplusplus
}
#endif

#endif
