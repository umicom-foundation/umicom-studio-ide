/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/product_centre.h
 *
 * PURPOSE:
 *   Compose Studio product identity, installed state and curated release feeds
 *   over the reusable Framework distribution service.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio supplies catalogue content and command IDs.  Framework owns package
 * policy, evidence, transaction, notification and toolkit-neutral view logic.
 */
#ifndef UMICOM_STUDIO_PRODUCT_CENTRE_H
#define UMICOM_STUDIO_PRODUCT_CENTRE_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioProductCentre UmiStudioProductCentre;

typedef struct UmiStudioProductCentreSnapshot {
    uint64_t revision;
    size_t marketplace_releases;
    size_t products;
    size_t installations;
    size_t policies;
    size_t evidence_records;
    size_t transactions;
    size_t available_updates;
    size_t notifications;
    size_t unread_notifications;
} UmiStudioProductCentreSnapshot;

UmiStatus umi_studio_product_centre_create(
    UmiStudioProductCentre **out_centre);
void umi_studio_product_centre_destroy(UmiStudioProductCentre *centre);
UmiDistributionService *umi_studio_product_centre_service(
    UmiStudioProductCentre *centre);
UmiStatus umi_studio_product_centre_snapshot(
    const UmiStudioProductCentre *centre,
    UmiStudioProductCentreSnapshot *out_snapshot);
UmiStatus umi_studio_product_centre_check_updates(
    UmiStudioProductCentre *centre,
    uint64_t timestamp_ms,
    size_t *out_available_updates);
UmiStatus umi_studio_product_centre_plan_update(
    UmiStudioProductCentre *centre,
    const char *release_id,
    uint64_t timestamp_ms,
    UmiDistributionDecision *out_decision,
    UmiDistributionTransaction *out_transaction);
UmiStatus umi_studio_product_centre_marketplace_view(
    UmiStudioProductCentre *centre,
    const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_product_centre_installed_view(
    UmiStudioProductCentre *centre,
    const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_product_centre_updates_view(
    UmiStudioProductCentre *centre,
    const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_product_centre_transactions_view(
    UmiStudioProductCentre *centre,
    const char *view_id,
    UmiUiViewModel **out_view);
UmiStatus umi_studio_product_centre_evidence_view(
    UmiStudioProductCentre *centre,
    const char *view_id,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
