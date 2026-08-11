/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/ai_evaluation.h
 *
 * PURPOSE:
 *   Calculate and report a simple Studio AI quality score.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio records evaluation evidence as Framework data so Helix can consume the same result without parsing UI text.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_EVALUATION_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_AI_EVALUATION_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_ai_evaluation(double score, double threshold, UmiAiEvaluation *evaluation);

#ifdef __cplusplus
}
#endif

#endif
