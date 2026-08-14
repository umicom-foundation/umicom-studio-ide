/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_distribution_centre_v2.c
 *
 * PURPOSE:
 *   Verify complete Studio release readiness and blocker reporting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio/distribution_centre_v2.h"

int main(void)
{
    UmiStudioDistributionCentreV2 centre;
    assert(umi_studio_distribution_centre_v2_init(
               &centre, "studio-0.23.0", "0.23.0") == UMI_STATUS_OK);
    assert(!umi_studio_distribution_centre_v2_ready(&centre));
    assert(umi_studio_distribution_centre_v2_blockers(&centre) > 0U);
    assert(umi_studio_supply_chain_v2_mark_local_complete(
               &centre.supply_chain) == UMI_STATUS_OK);
    assert(umi_studio_supply_chain_v2_approve_signature(
               &centre.supply_chain, "Umicom Foundation") == UMI_STATUS_OK);
    assert(umi_studio_distribution_centre_v2_refresh(&centre) == UMI_STATUS_OK);
    assert(umi_studio_distribution_centre_v2_ready(&centre));
    assert(umi_studio_distribution_centre_v2_blockers(&centre) == 0U);
    return 0;
}
