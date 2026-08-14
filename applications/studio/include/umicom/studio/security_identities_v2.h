/* Umicom Studio IDE | Security identity administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_IDENTITIES_V2_H
#define UMICOM_STUDIO_SECURITY_IDENTITIES_V2_H
#include "umicom/studio/security_centre_v2.h"
UmiStatus umi_studio_security_identity_add(UmiStudioSecurityCentre *centre,const UmiSecurityIdentityProfile *identity);
UmiStatus umi_studio_security_identity_set_enabled(UmiStudioSecurityCentre *centre,const char *identity_id,bool enabled);
const UmiSecurityIdentityProfile *umi_studio_security_identity_find(const UmiStudioSecurityCentre *centre,const char *identity_id);
#endif
