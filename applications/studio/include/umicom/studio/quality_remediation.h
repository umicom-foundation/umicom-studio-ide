/* Umicom Studio IDE | Quality remediation guidance | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_REMEDIATION_H
#define UMICOM_STUDIO_QUALITY_REMEDIATION_H
#include "umicom/studio/quality_centre.h"
UmiStatus umi_studio_quality_remediation_refresh(UmiStudioQualityCentre *centre);
const UmiCodeGuardRemediationAction *umi_studio_quality_remediation_at(const UmiStudioQualityCentre *centre,size_t index);
#endif
