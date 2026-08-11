/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/market_workspace.c
 *
 * PURPOSE:
 *   Track the active instrument, last quote and market state for the Trader workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/market_workspace.h"

void umi_trader_market_workspace_init(UmiTraderMarketWorkspace *workspace)
{
    if (workspace != NULL) {
        *workspace = (UmiTraderMarketWorkspace){0};
        workspace->market_state = UMI_MARKET_CLOSED;
    }
}

UmiStatus
umi_trader_market_workspace_set_instrument(UmiTraderMarketWorkspace *workspace,
                                           const UmiInstrument *instrument)
{
    if (workspace == NULL || !umi_instrument_valid(instrument)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    workspace->active_instrument = *instrument;
    workspace->has_instrument = 1;
    return UMI_STATUS_OK;
}

UmiStatus
umi_trader_market_workspace_on_quote(UmiTraderMarketWorkspace *workspace,
                                     const UmiQuote *quote)
{
    if (workspace == NULL ||
        !umi_quote_valid(quote) ||
        !workspace->has_instrument ||
        !umi_instrument_same(&workspace->active_instrument,
                             &quote->instrument)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    workspace->last_quote = *quote;
    workspace->has_quote = 1;
    return UMI_STATUS_OK;
}
