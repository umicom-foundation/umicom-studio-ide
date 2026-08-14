/* Umicom Studio IDE | Quality Centre commands v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/quality_commands.h"
#include "umicom/studio/quality_compatibility.h"
#include "umicom/studio/quality_duplicates.h"
#include "umicom/studio/quality_evidence.h"
#include "umicom/studio/quality_remediation.h"
UmiStatus umi_studio_quality_seed_evidence(UmiStudioQualityCentre *centre)
{
    UmiCodeGuardDuplicateFile first = {"src/common_a.c",55U,77U,240U};
    UmiCodeGuardDuplicateFile second = {"src/common_b.c",55U,77U,240U};
    UmiCodeGuardApiBaseline expected;
    UmiCodeGuardApiBaseline current;
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_studio_quality_record_check(centre,"build.windows",UMI_CODEGUARD_EVIDENCE_BUILD,true,0U,0U,"CMakeLists.txt",1U,"Windows build completed","Review compiler output when this check fails");
    if (status == UMI_STATUS_OK) status = umi_studio_quality_record_check(centre,"tests.native",UMI_CODEGUARD_EVIDENCE_TEST,true,0U,0U,"tests",1U,"Native tests completed","Open the failing test and its captured diagnostics");
    if (status == UMI_STATUS_OK) status = umi_studio_quality_record_architecture_dependency(centre,"applications/studio/src/app/quality.c","framework/include/umicom/codeguard/codeguard.h");
    if (status == UMI_STATUS_OK) status = umi_studio_quality_duplicate_add(centre,&first,&second);
    if (status != UMI_STATUS_OK) return status;
    status = umi_codeguard_api_baseline_init(&expected,"framework",1U,0U);
    if (status == UMI_STATUS_OK) status = umi_codeguard_api_baseline_init(&current,"framework",1U,1U);
    if (status == UMI_STATUS_OK) status = umi_codeguard_api_baseline_add(&expected,"umi_framework_start",101U,false);
    if (status == UMI_STATUS_OK) status = umi_codeguard_api_baseline_add(&current,"umi_framework_start",101U,false);
    if (status == UMI_STATUS_OK) status = umi_codeguard_api_baseline_add(&current,"umi_quality_session",102U,false);
    if (status == UMI_STATUS_OK) status = umi_studio_quality_compare_api(centre,&expected,&current);
    return status;
}
UmiStatus umi_studio_quality_execute(UmiStudioQualityCentre *centre,UmiStudioQualityCommand command,UmiCodeGuardQualityEvaluation *out_evaluation)
{
    switch (command) {
        case UMI_STUDIO_QUALITY_COMMAND_SEED_EVIDENCE: return umi_studio_quality_seed_evidence(centre);
        case UMI_STUDIO_QUALITY_COMMAND_EVALUATE:
            if (out_evaluation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
            return umi_studio_quality_evaluate(centre,out_evaluation);
        case UMI_STUDIO_QUALITY_COMMAND_REFRESH_REMEDIATION: return umi_studio_quality_remediation_refresh(centre);
        default: return UMI_STATUS_NOT_FOUND;
    }
}
