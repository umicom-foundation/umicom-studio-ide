/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_market_workspace.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader market workspace foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The product test checks composition while reusable trading rules are covered separately by Framework tests.
 */

#include <assert.h>
#include <stdio.h>
#include "umicom/trader/trader.h"

static UmiInstrument instrument_fixture(void)
{
    UmiInstrument instrument = {0};
    (void)snprintf(instrument.instrument_id.value,
                   sizeof(instrument.instrument_id.value),
                   "%s",
                   "CME.NQ");
    (void)snprintf(instrument.symbol,
                   sizeof(instrument.symbol),
                   "%s",
                   "NQ");
    (void)snprintf(instrument.venue,
                   sizeof(instrument.venue),
                   "%s",
                   "CME");
    (void)snprintf(instrument.currency.code,
                   sizeof(instrument.currency.code),
                   "%s",
                   "USD");
    instrument.multiplier = 20.0;
    return instrument;
}

int main(void)
{
    UmiTraderMarketWorkspace workspace;
    umi_trader_market_workspace_init(&workspace);

    const UmiInstrument instrument = instrument_fixture();
    assert(umi_trader_market_workspace_set_instrument(&workspace,
                                                      &instrument) ==
           UMI_STATUS_OK);

    UmiQuote quote = {0};
    quote.instrument = instrument;
    quote.bid = 100.0;
    quote.ask = 101.0;
    quote.bid_size = 1.0;
    quote.ask_size = 1.0;

    assert(umi_trader_market_workspace_on_quote(&workspace,
                                                &quote) ==
           UMI_STATUS_OK);
    return 0;
}
