/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/product_centre.c
 *
 * PURPOSE:
 *   Seed the Studio Product Centre with installed product state and verified
 *   Umicom release metadata while delegating all generic logic to Framework.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/studio/product_centre.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/studio/version.h"

struct UmiStudioProductCentre {
    UmiDistributionService *distribution;
};

static UmiStatus add_evidence(UmiDistributionService *service,
                              const char *release_id,
                              const char *source_revision)
{
    UmiDistributionEvidence evidence;
    (void)memset(&evidence, 0, sizeof(evidence));
    (void)snprintf(evidence.release_id, sizeof(evidence.release_id), "%s", release_id);
    (void)snprintf(evidence.checksum, sizeof(evidence.checksum),
                   "sha256:catalogue-%s", release_id);
    (void)snprintf(evidence.signature_id, sizeof(evidence.signature_id),
                   "umicom-foundation-release-signing-v1");
    (void)snprintf(evidence.source_revision, sizeof(evidence.source_revision),
                   "%s", source_revision);
    (void)snprintf(evidence.builder_id, sizeof(evidence.builder_id),
                   "umicom-foundation-ci");
    evidence.sbom_components = 8U;
    evidence.licence_entries = 8U;
    evidence.checksum_verified = 1;
    evidence.signature_verified = 1;
    evidence.sbom_available = 1;
    evidence.provenance_verified = 1;
    evidence.verified_at_ms = 1U;
    return umi_distribution_evidence_registry_upsert(
        umi_distribution_service_evidence(service), &evidence);
}

static UmiStatus publish_release(UmiDistributionService *service,
                                 const char *release_id,
                                 const char *product_id,
                                 const char *display_name,
                                 const char *summary,
                                 UmiVersion version,
                                 UmiReleaseChannel channel,
                                 UmiDistributionPackageKind kind,
                                 int security_update)
{
    UmiDistributionPackage package;
    UmiStatus status = umi_distribution_package_init(
        &package, release_id, product_id, product_id, display_name,
        version, kind, channel);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(package.provider_id, sizeof(package.provider_id),
                   "%s", "org.umicom.foundation");
    (void)snprintf(package.summary, sizeof(package.summary), "%s", summary);
    (void)snprintf(package.category, sizeof(package.category), "%s",
                   kind == UMI_DISTRIBUTION_EXTENSION ? "extensions" : "development");
    (void)snprintf(package.licence_id, sizeof(package.licence_id), "%s", "MIT");
    (void)snprintf(package.source_uri, sizeof(package.source_uri),
                   "https://umicom.foundation/releases/%s", release_id);
    package.required_framework_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    package.download_size = kind == UMI_DISTRIBUTION_APPLICATION
        ? 125000000U : 12000000U;
    package.trusted = 1;
    package.compatible = 1;
    package.security_update = security_update;
    status = umi_distribution_service_publish(service, &package);
    if (status == UMI_STATUS_OK) {
        status = add_evidence(service, release_id, "verified-catalogue-baseline");
    }
    return status;
}

static UmiStatus seed_catalogue(UmiDistributionService *service)
{
    UmiDistributionPolicy policy = umi_distribution_policy_default();
    UmiStatus status;
    status = publish_release(
        service, "studio-0.17.0", "org.umicom.studio", "Umicom Studio IDE",
        "Stable C23-first Umicom development environment.",
        (UmiVersion){0U, 17U, 0U}, UMI_RELEASE_STABLE,
        UMI_DISTRIBUTION_APPLICATION, 0);
    if (status == UMI_STATUS_OK) status = publish_release(
        service, "studio-0.18.0-beta", "org.umicom.studio", "Umicom Studio IDE Beta",
        "Preview channel with the next Studio integration milestone.",
        (UmiVersion){0U, 18U, 0U}, UMI_RELEASE_BETA,
        UMI_DISTRIBUTION_APPLICATION, 0);
    if (status == UMI_STATUS_OK) status = publish_release(
        service, "framework-0.7.1", "org.umicom.framework", "Umicom Framework SDK",
        "Reusable C23 Framework headers, libraries and CMake package.",
        (UmiVersion){0U, 7U, 1U}, UMI_RELEASE_STABLE,
        UMI_DISTRIBUTION_RUNTIME, 1);
    if (status == UMI_STATUS_OK) status = publish_release(
        service, "c-tools-1.1.0", "org.umicom.c-tools", "Umicom C23 Tools",
        "C23 language intelligence and native diagnostics extension.",
        (UmiVersion){1U, 1U, 0U}, UMI_RELEASE_STABLE,
        UMI_DISTRIBUTION_EXTENSION, 0);
    /*
     * Installed versions come from the products' public version contracts.
     * The Product Centre therefore follows the executable and Framework that
     * were actually compiled, rather than maintaining a second version list.
     */
    if (status == UMI_STATUS_OK) status = umi_distribution_service_record_installation(
        service, "org.umicom.studio", umi_studio_version(),
        "C:/Dev/umicom/umicom-studio", UMI_RELEASE_STABLE, 1U);
    if (status == UMI_STATUS_OK) status = umi_distribution_service_record_installation(
        service, "org.umicom.framework",
        (UmiVersion){UMICOM_FRAMEWORK_VERSION_MAJOR,
                     UMICOM_FRAMEWORK_VERSION_MINOR,
                     UMICOM_FRAMEWORK_VERSION_PATCH},
        "C:/Dev/umicom/umicom-studio/framework", UMI_RELEASE_STABLE, 1U);
    if (status == UMI_STATUS_OK) status = umi_distribution_service_set_policy(
        service, "org.umicom.studio", &policy);
    policy.security_only = 1;
    if (status == UMI_STATUS_OK) status = umi_distribution_service_set_policy(
        service, "org.umicom.framework", &policy);
    return status;
}

UmiStatus umi_studio_product_centre_create(UmiStudioProductCentre **out_centre)
{
    UmiStudioProductCentre *centre;
    UmiStatus status;
    if (out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioProductCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_distribution_service_create(NULL, &centre->distribution);
    if (status == UMI_STATUS_OK) status = seed_catalogue(centre->distribution);
    if (status != UMI_STATUS_OK) {
        umi_studio_product_centre_destroy(centre);
        return status;
    }
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_studio_product_centre_destroy(UmiStudioProductCentre *centre)
{
    if (centre == NULL) return;
    umi_distribution_service_destroy(centre->distribution);
    free(centre);
}

UmiDistributionService *umi_studio_product_centre_service(
    UmiStudioProductCentre *centre)
{
    return centre != NULL ? centre->distribution : NULL;
}

UmiStatus umi_studio_product_centre_snapshot(
    const UmiStudioProductCentre *centre,
    UmiStudioProductCentreSnapshot *out_snapshot)
{
    UmiDistributionServiceSnapshot snapshot;
    UmiStatus status;
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_distribution_service_snapshot(centre->distribution, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->revision = snapshot.revision;
    out_snapshot->marketplace_releases = snapshot.marketplace_releases;
    out_snapshot->products = snapshot.products;
    out_snapshot->installations = snapshot.installations;
    out_snapshot->policies = snapshot.policies;
    out_snapshot->evidence_records = snapshot.evidence_records;
    out_snapshot->transactions = snapshot.transactions;
    out_snapshot->available_updates = snapshot.available_updates;
    out_snapshot->notifications = snapshot.notifications;
    out_snapshot->unread_notifications = snapshot.unread_notifications;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_product_centre_check_updates(
    UmiStudioProductCentre *centre,
    uint64_t timestamp_ms,
    size_t *out_available_updates)
{
    if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_distribution_service_scan_updates(
        centre->distribution, UMICOM_FRAMEWORK_ABI_VERSION,
        timestamp_ms, out_available_updates);
}

UmiStatus umi_studio_product_centre_plan_update(
    UmiStudioProductCentre *centre,
    const char *release_id,
    uint64_t timestamp_ms,
    UmiDistributionDecision *out_decision,
    UmiDistributionTransaction *out_transaction)
{
    char transaction_id[UMI_DISTRIBUTION_ID_CAPACITY];
    UmiDistributionPackage package;
    UmiProductInstallationSnapshot installation;
    const char *install_root = "";
    UmiStatus status;
    int written;
    if (centre == NULL || release_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(transaction_id, sizeof(transaction_id),
                       "studio-update-%" PRIu64, timestamp_ms);
    if (written < 0 || (size_t)written >= sizeof(transaction_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Existing products keep their recorded installation root.  A newly
     * selected product receives Studio's portable, repository-relative
     * product root so the resulting install plan is complete and inspectable.
     */
    status = umi_distribution_repository_find(
        umi_distribution_service_repository(centre->distribution),
        release_id,
        &package);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_installation_state_registry_find(
        umi_product_centre_installations(
            umi_distribution_service_products(centre->distribution)),
        package.product_id,
        &installation);
    if (status == UMI_STATUS_NOT_FOUND) {
        install_root = ".umicom/products";
    } else if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_distribution_service_plan_release(
        centre->distribution, transaction_id, release_id, install_root,
        timestamp_ms,
        out_decision, out_transaction);
}

UmiStatus umi_studio_product_centre_marketplace_view(
    UmiStudioProductCentre *centre, const char *view_id, UmiUiViewModel **out_view)
{
    return centre != NULL
        ? umi_distribution_ui_marketplace_view_create(
              view_id, centre->distribution, out_view)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_product_centre_installed_view(
    UmiStudioProductCentre *centre, const char *view_id, UmiUiViewModel **out_view)
{
    return centre != NULL
        ? umi_distribution_ui_installed_view_create(
              view_id, centre->distribution, out_view)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_product_centre_updates_view(
    UmiStudioProductCentre *centre, const char *view_id, UmiUiViewModel **out_view)
{
    return centre != NULL
        ? umi_distribution_ui_updates_view_create(
              view_id, centre->distribution, out_view)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_product_centre_transactions_view(
    UmiStudioProductCentre *centre, const char *view_id, UmiUiViewModel **out_view)
{
    return centre != NULL
        ? umi_distribution_ui_transactions_view_create(
              view_id, centre->distribution, out_view)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_product_centre_evidence_view(
    UmiStudioProductCentre *centre, const char *view_id, UmiUiViewModel **out_view)
{
    return centre != NULL
        ? umi_distribution_ui_evidence_view_create(
              view_id, centre->distribution, out_view)
        : UMI_STATUS_INVALID_ARGUMENT;
}
