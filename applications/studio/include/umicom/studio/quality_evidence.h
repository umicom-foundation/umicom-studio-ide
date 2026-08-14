/* Umicom Studio IDE | Quality evidence composition | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_QUALITY_EVIDENCE_H
#define UMICOM_STUDIO_QUALITY_EVIDENCE_H
#include "umicom/studio/quality_centre.h"
UmiStatus umi_studio_quality_record_check(UmiStudioQualityCentre *centre,const char *id,UmiCodeGuardEvidenceKind kind,bool passed,size_t observed,size_t allowed,const char *path,size_t line,const char *summary,const char *remediation);
UmiStatus umi_studio_quality_record_architecture_dependency(UmiStudioQualityCentre *centre,const char *source_path,const char *target_path);
const UmiCodeGuardEvidence *umi_studio_quality_find_evidence(const UmiStudioQualityCentre *centre,const char *evidence_id);
#endif
