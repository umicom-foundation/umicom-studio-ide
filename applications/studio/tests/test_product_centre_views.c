/*-----------------------------------------------------------------------------
 * Umicom Studio IDE Tests
 * File: applications/studio/tests/test_product_centre_views.c
 *
 * PURPOSE:
 *   Verify all Product Centre pane factories produce live toolkit-neutral
 *   Framework view models.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/studio/product_centre.h"

int main(void)
{
    UmiStudioProductCentre *centre = NULL;
    UmiUiViewModel *view = NULL;
    size_t updates = 0U;
    assert(umi_studio_product_centre_create(&centre) == UMI_STATUS_OK);
    assert(umi_studio_product_centre_check_updates(
        centre, 100U, &updates) == UMI_STATUS_OK);
    assert(umi_studio_product_centre_marketplace_view(
        centre, "studio.product-marketplace", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_product_centre_installed_view(
        centre, "studio.products-installed", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_product_centre_updates_view(
        centre, "studio.product-updates", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_product_centre_transactions_view(
        centre, "studio.product-transactions", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_studio_product_centre_evidence_view(
        centre, "studio.product-evidence", &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_studio_product_centre_destroy(centre);
    return 0;
}
