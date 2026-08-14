/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_distribution_centre.c
 *
 * PURPOSE:
 *   Verify complete Studio release readiness and blocker reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/distribution_centre.h"

int main(void)
{
    UmiStudioDistributionCentre centre;
    assert(umi_studio_distribution_centre_init(
               &centre, "studio-0.23.0", "0.23.0") == UMI_STATUS_OK);
    assert(!umi_studio_distribution_centre_ready(&centre));
    assert(umi_studio_distribution_centre_blockers(&centre) > 0U);
    assert(umi_studio_supply_chain_mark_local_complete(
               &centre.supply_chain) == UMI_STATUS_OK);
    assert(umi_studio_supply_chain_approve_signature(
               &centre.supply_chain, "Umicom Foundation") == UMI_STATUS_OK);
    assert(umi_studio_distribution_centre_refresh(&centre) == UMI_STATUS_OK);
    assert(umi_studio_distribution_centre_ready(&centre));
    assert(umi_studio_distribution_centre_blockers(&centre) == 0U);
    return 0;
}
