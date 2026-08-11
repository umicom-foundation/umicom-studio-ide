/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_broker_settings.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader broker settings foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The product test checks composition while reusable trading rules are covered separately by Framework tests.
 */

#include <assert.h>
#include "umicom/trader/trader.h"

int main(void)
{
    const UmiIbkrConnectionSettings paper =
        umi_trader_default_ibkr_settings(UMI_TRADING_PAPER);
    assert(paper.port == 7497U);
    assert(umi_ibkr_settings_valid(&paper));

    const UmiIbkrConnectionSettings live =
        umi_trader_default_ibkr_settings(UMI_TRADING_LIVE);
    assert(live.port == 7496U);
    return 0;
}
