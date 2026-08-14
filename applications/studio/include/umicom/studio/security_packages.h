/* Umicom Studio IDE | Package signing trust v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_PACKAGES_H
#define UMICOM_STUDIO_SECURITY_PACKAGES_H
#include "umicom/studio/security_centre.h"
UmiStatus umi_studio_security_signer_add(UmiStudioSecurityCentre *centre,const UmiSecurityTrustedSigner *signer);
UmiStatus umi_studio_security_package_verify(const UmiStudioSecurityCentre *centre,const UmiSecurityPackageEvidence *evidence,UmiSecurityEnvironment environment,UmiSecurityGovernanceDecision *out_decision);
#endif
