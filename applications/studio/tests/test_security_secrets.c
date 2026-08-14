/* Umicom Studio IDE Tests | Security secret references v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "security_centre_fixture.h"
#include "umicom/studio/security_secrets.h"

int main(void)
{
    UmiStudioSecurityCentre *centre = umi_test_seeded_security_centre();
    const UmiSecuritySecretMetadata *secret =
        umi_studio_security_secret_find(centre,"vault://studio/signing-key");
    char redacted[32U];
    char visible[32U];
    assert(secret != NULL && strcmp(secret->secret.provider,"vault") == 0);
    assert(strcmp(secret->secret.name,"studio/signing-key") == 0);
    assert(umi_studio_security_redact_field(
        centre,"token","raw-value",redacted,sizeof(redacted)) == UMI_STATUS_OK);
    assert(umi_studio_security_redact_field(
        centre,"purpose","metadata",visible,sizeof(visible)) == UMI_STATUS_OK);
    assert(strcmp(redacted,"[REDACTED]") == 0 && strcmp(visible,"metadata") == 0);
    umi_studio_security_centre_destroy(centre);
    return 0;
}
