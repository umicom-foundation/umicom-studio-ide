/* Umicom Studio IDE | Duplicate review evidence | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_duplicates.h"
UmiStatus umi_studio_quality_duplicate_add(UmiStudioQualityCentre *centre,const UmiCodeGuardDuplicateFile *first,const UmiCodeGuardDuplicateFile *second)
{
    UmiCodeGuardDuplicateCandidate candidate;
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_duplicate_candidate_build(first,second,&candidate);
    if (status == UMI_STATUS_OK) status = umi_codeguard_duplicate_review_add(&centre->session.duplicates,&candidate);
    if (status == UMI_STATUS_OK) { centre->session.revision += 1U; centre->revision += 1U; }
    return status;
}
UmiStatus umi_studio_quality_duplicate_decide(UmiStudioQualityCentre *centre,size_t index,UmiCodeGuardCandidateDisposition disposition)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_duplicate_review_disposition(&centre->session.duplicates,index,disposition);
    if (status == UMI_STATUS_OK) { centre->session.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiCodeGuardDuplicateCandidate *umi_studio_quality_duplicate_at(const UmiStudioQualityCentre *centre,size_t index)
{
    return centre != NULL && index < centre->session.duplicates.count ? &centre->session.duplicates.items[index] : NULL;
}
