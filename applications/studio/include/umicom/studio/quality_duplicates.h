/* Umicom Studio IDE | Duplicate review evidence | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_DUPLICATES_H
#define UMICOM_STUDIO_QUALITY_DUPLICATES_H
#include "umicom/studio/quality_centre.h"
UmiStatus umi_studio_quality_duplicate_add(UmiStudioQualityCentre *centre,const UmiCodeGuardDuplicateFile *first,const UmiCodeGuardDuplicateFile *second);
UmiStatus umi_studio_quality_duplicate_decide(UmiStudioQualityCentre *centre,size_t index,UmiCodeGuardCandidateDisposition disposition);
const UmiCodeGuardDuplicateCandidate *umi_studio_quality_duplicate_at(const UmiStudioQualityCentre *centre,size_t index);
#endif
