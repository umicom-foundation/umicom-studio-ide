/* Umicom Studio IDE | Security identity administration v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_identities.h"
UmiStatus umi_studio_security_identity_add(UmiStudioSecurityCentre *centre,const UmiSecurityIdentityProfile *identity)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_identity_directory_add(&centre->governance.identities,identity);
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
UmiStatus umi_studio_security_identity_set_enabled(UmiStudioSecurityCentre *centre,const char *identity_id,bool enabled)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_identity_directory_set_enabled(&centre->governance.identities,identity_id,enabled);
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiSecurityIdentityProfile *umi_studio_security_identity_find(const UmiStudioSecurityCentre *centre,const char *identity_id) { return centre == NULL ? NULL : umi_security_identity_directory_find(&centre->governance.identities,identity_id); }
