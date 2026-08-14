/* Umicom Studio IDE | API compatibility inspection | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_COMPATIBILITY_H
#define UMICOM_STUDIO_QUALITY_COMPATIBILITY_H
#include "umicom/studio/quality_centre.h"
UmiStatus umi_studio_quality_compare_api(UmiStudioQualityCentre *centre,const UmiCodeGuardApiBaseline *expected,const UmiCodeGuardApiBaseline *current);
const UmiCodeGuardApiChange *umi_studio_quality_api_change_at(const UmiStudioQualityCentre *centre,size_t index);
#endif
