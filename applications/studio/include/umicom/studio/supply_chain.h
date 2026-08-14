/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/supply_chain.h
 *
 * PURPOSE:
 *   Track Studio checksums, signatures, licences, SBOM and provenance gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SUPPLY_CHAIN_H
#define UMICOM_STUDIO_SUPPLY_CHAIN_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioSupplyChain {
    UmiReleaseEvidence evidence;
    char checksum_algorithm[32];
    char sbom_format[32];
    char signer[UMI_DELIVERY_ID_CAPACITY];
} UmiStudioSupplyChain;

void umi_studio_supply_chain_init(UmiStudioSupplyChain *supply_chain);
UmiStatus umi_studio_supply_chain_mark_local_complete(
    UmiStudioSupplyChain *supply_chain);
UmiStatus umi_studio_supply_chain_approve_signature(
    UmiStudioSupplyChain *supply_chain,
    const char *signer);
int umi_studio_supply_chain_ready(
    const UmiStudioSupplyChain *supply_chain,
    UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif
#endif
