/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/market_workspace.h
 *
 * PURPOSE:
 *   Track the active instrument, last quote and market state for the Trader workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework market workspace capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_MARKET_WORKSPACE_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_MARKET_WORKSPACE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderMarketWorkspace { UmiInstrument active_instrument; UmiQuote last_quote; UmiMarketState market_state; int has_instrument; int has_quote; } UmiTraderMarketWorkspace;
void umi_trader_market_workspace_init(UmiTraderMarketWorkspace *workspace);
UmiStatus umi_trader_market_workspace_set_instrument(UmiTraderMarketWorkspace *workspace, const UmiInstrument *instrument);
UmiStatus umi_trader_market_workspace_on_quote(UmiTraderMarketWorkspace *workspace, const UmiQuote *quote);
#ifdef __cplusplus
}
#endif
#endif
