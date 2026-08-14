/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/services.h
 *
 * PURPOSE:
 *   Own the shared Trader service container over Framework trading services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework services capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_SERVICES_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_SERVICES_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderServices {
    UmiOms oms;
    UmiWatchlist watchlist;
    UmiResearchStats research_stats;
    UmiPositionBook positions;
    UmiSimulatedBrokerStorage simulated_broker_storage;
    UmiBroker broker;
    int broker_created;
} UmiTraderServices;
UmiStatus umi_trader_services_init(UmiTraderServices *services);
void umi_trader_services_destroy(UmiTraderServices *services);
#ifdef __cplusplus
}
#endif
#endif
