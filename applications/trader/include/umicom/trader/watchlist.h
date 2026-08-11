/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/watchlist.h
 *
 * PURPOSE:
 *   Compose Trader watchlist operations over Framework instruments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework watchlist capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_WATCHLIST_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_WATCHLIST_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/services.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_trader_watchlist_add(UmiTraderServices *services, const UmiInstrument *instrument);
#ifdef __cplusplus
}
#endif
#endif
