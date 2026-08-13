/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_product_centre.c
 *
 * PURPOSE:
 *   Verify Studio composes the Framework Product, Marketplace and Update
 *   service with verified catalogue and installed-product state.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/product_centre.h"

int main(void)
{
    UmiStudioProductCentre *centre = NULL;
    UmiStudioProductCentreSnapshot snapshot;
    UmiDistributionDecision decision;
    UmiDistributionTransaction transaction;
    size_t updates = 0U;
    assert(umi_studio_product_centre_create(&centre) == UMI_STATUS_OK);
    assert(umi_studio_product_centre_snapshot(centre, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.marketplace_releases == 4U);
    assert(snapshot.products == 3U);
    assert(snapshot.installations == 2U);
    assert(snapshot.policies == 2U);
    assert(snapshot.evidence_records == 4U);
    assert(umi_studio_product_centre_check_updates(
        centre, 100U, &updates) == UMI_STATUS_OK);
    assert(updates == 2U);
    assert(umi_studio_product_centre_plan_update(
        centre, "studio-0.17.0", 200U, &decision, &transaction) == UMI_STATUS_OK);
    assert(decision.update_available);
    assert(transaction.action == UMI_DISTRIBUTION_UPDATE);
    assert(transaction.reversible);
    assert(umi_studio_product_centre_plan_update(
        centre, "c-tools-1.1.0", 300U, &decision, &transaction) ==
        UMI_STATUS_OK);
    assert(decision.installation_required);
    assert(transaction.action == UMI_DISTRIBUTION_INSTALL);
    assert(strcmp(transaction.install_root, ".umicom/products") == 0);
    umi_studio_product_centre_destroy(centre);
    return 0;
}
