/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_framework_bootstrap.c
 *---------------------------------------------------------------------------*/
#include "umicom/studio/bootstrap.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(bootstrap != NULL);
    assert(strcmp(umi_studio_bootstrap_application_name(bootstrap),
                  "Umicom Studio IDE") == 0);
    assert(umi_studio_bootstrap_module_count(bootstrap) == 1U);
    assert(umi_studio_bootstrap_start(bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_bootstrap_stop(bootstrap) == UMI_STATUS_OK);
    umi_studio_bootstrap_destroy(bootstrap);
    return 0;
}
