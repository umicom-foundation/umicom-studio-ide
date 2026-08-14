/* Umicom Studio IDE | Security secret inventory v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_SECURITY_SECRETS_H
#define UMICOM_STUDIO_SECURITY_SECRETS_H
#include "umicom/studio/security_centre.h"
UmiStatus umi_studio_security_secret_add(UmiStudioSecurityCentre *centre,const UmiSecuritySecretMetadata *metadata);
const UmiSecuritySecretMetadata *umi_studio_security_secret_find(const UmiStudioSecurityCentre *centre,const char *reference);
UmiStatus umi_studio_security_redact_field(const UmiStudioSecurityCentre *centre,const char *field_name,const char *value,char *out_value,size_t capacity);
#endif
