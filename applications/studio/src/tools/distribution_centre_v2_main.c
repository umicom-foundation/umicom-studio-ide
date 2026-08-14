/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/distribution_centre_v2_main.c
 *
 * PURPOSE:
 *   Provide a native command for inspecting complete release readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/studio/distribution_centre_v2.h"

int main(void)
{
    UmiStudioDistributionCentreV2 centre;
    if (umi_studio_distribution_centre_v2_init(
            &centre, "studio-0.23.0", "0.23.0") != UMI_STATUS_OK) return 1;
    (void)umi_studio_supply_chain_v2_mark_local_complete(&centre.supply_chain);
    (void)umi_studio_supply_chain_v2_approve_signature(
        &centre.supply_chain, "Umicom Foundation");
    (void)umi_studio_distribution_centre_v2_refresh(&centre);
    printf("Umicom Studio Distribution Centre\n");
    printf("Release: %s\n", centre.release.release_id);
    printf("Installers: %zu\n", centre.release.installer_count);
    printf("Blockers: %zu\n",
           umi_studio_distribution_centre_v2_blockers(&centre));
    return umi_studio_distribution_centre_v2_ready(&centre) ? 0 : 1;
}
