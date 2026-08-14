/* Umicom Studio IDE | Security secret inventory v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_secrets.h"
UmiStatus umi_studio_security_secret_add(UmiStudioSecurityCentre *centre,const UmiSecuritySecretMetadata *metadata)
{
    UmiStatus status;
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_security_secret_inventory_add(&centre->governance.secrets,metadata);
    if (status == UMI_STATUS_OK) { centre->governance.revision += 1U; centre->revision += 1U; }
    return status;
}
const UmiSecuritySecretMetadata *umi_studio_security_secret_find(const UmiStudioSecurityCentre *centre,const char *reference) { return centre == NULL ? NULL : umi_security_secret_inventory_find(&centre->governance.secrets,reference); }
UmiStatus umi_studio_security_redact_field(const UmiStudioSecurityCentre *centre,const char *field_name,const char *value,char *out_value,size_t capacity) { return centre == NULL ? UMI_STATUS_INVALID_ARGUMENT : umi_security_redaction_copy(&centre->governance.redaction,field_name,value,out_value,capacity); }
