/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/distribution_centre_main.c
 *
 * PURPOSE:
 *   Provide a native command for inspecting complete release readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/studio/distribution_centre.h"

int main(void)
{
    UmiStudioDistributionCentre centre;
    if (umi_studio_distribution_centre_init(
            &centre, "studio-0.23.0", "0.23.0") != UMI_STATUS_OK) return 1;
    (void)umi_studio_supply_chain_mark_local_complete(&centre.supply_chain);
    (void)umi_studio_supply_chain_approve_signature(
        &centre.supply_chain, "Umicom Foundation");
    (void)umi_studio_distribution_centre_refresh(&centre);
    printf("Umicom Studio Distribution Centre\n");
    printf("Release: %s\n", centre.release.release_id);
    printf("Installers: %zu\n", centre.release.installer_count);
    printf("Blockers: %zu\n",
           umi_studio_distribution_centre_blockers(&centre));
    return umi_studio_distribution_centre_ready(&centre) ? 0 : 1;
}
