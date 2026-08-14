/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/supply_chain_v2.h
 *
 * PURPOSE:
 *   Track Studio checksums, signatures, licences, SBOM and provenance gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SUPPLY_CHAIN_V2_H
#define UMICOM_STUDIO_SUPPLY_CHAIN_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSupplyChainV2 {
    UmiReleaseEvidenceV2 evidence;
    char checksum_algorithm[32];
    char sbom_format[32];
    char signer[UMI_DELIVERY_ID_CAPACITY];
} UmiStudioSupplyChainV2;

void umi_studio_supply_chain_v2_init(UmiStudioSupplyChainV2 *supply_chain);
UmiStatus umi_studio_supply_chain_v2_mark_local_complete(
    UmiStudioSupplyChainV2 *supply_chain);
UmiStatus umi_studio_supply_chain_v2_approve_signature(
    UmiStudioSupplyChainV2 *supply_chain,
    const char *signer);
int umi_studio_supply_chain_v2_ready(
    const UmiStudioSupplyChainV2 *supply_chain,
    UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif
#endif
