/* Umicom Studio IDE | Combined quality gates v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_GATES_H
#define UMICOM_STUDIO_QUALITY_GATES_H
#include "umicom/studio/quality_centre.h"
UmiStatus umi_studio_quality_set_policy(UmiStudioQualityCentre *centre,const UmiCodeGuardQualityPolicy *policy);
UmiStatus umi_studio_quality_evaluate(UmiStudioQualityCentre *centre,UmiCodeGuardQualityEvaluation *out_evaluation);
#endif
