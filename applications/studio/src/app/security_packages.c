/* Umicom Studio IDE | Package signing trust v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_packages.h"

UmiStatus umi_studio_security_signer_add(UmiStudioSecurityCentre *centre,
                                         const UmiSecurityTrustedSigner *signer)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_signer_store_add(&centre->governance.signers,signer);
    if (status == UMI_STATUS_OK) {
        centre->governance.revision += 1U;
        centre->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_security_package_verify(
    const UmiStudioSecurityCentre *centre,
    const UmiSecurityPackageEvidence *evidence,
    UmiSecurityEnvironment environment,
    UmiSecurityGovernanceDecision *out_decision)
{
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_security_package_trust_evaluate(&centre->governance.signers,
                                               evidence,
                                               environment,
                                               out_decision);
}
