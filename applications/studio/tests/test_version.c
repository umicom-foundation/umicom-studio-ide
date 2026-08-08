#include "umicom/studio/version.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiVersion version = umi_studio_version();
    assert(version.major == 0U);
    assert(version.minor == 11U);
    assert(version.patch == 1U);
    assert(strcmp(umi_studio_version_string(), "0.11.1") == 0);
    return 0;
}
