/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_version.c
 *
 * PURPOSE:
 *   Verify the public Studio version structure and string remain synchronised
 *   with the 0.17.0 release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/version.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiVersion version = umi_studio_version();
    assert(version.major == 0U);
    assert(version.minor == 16U);
    assert(version.patch == 0U);
    assert(strcmp(umi_studio_version_string(), "0.17.0") == 0);
    return 0;
}
