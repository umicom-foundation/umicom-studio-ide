/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_status.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader status foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The product test checks composition while reusable trading rules are covered separately by Framework tests.
 */

#include <assert.h>
#include <string.h>
#include "umicom/trader/trader.h"

int main(void)
{
    UmiTraderServices services;
    assert(umi_trader_services_init(&services) == UMI_STATUS_OK);

    char text[256];
    assert(umi_trader_status_text(&services,
                                  text,
                                  sizeof(text)) ==
           UMI_STATUS_OK);
    assert(strstr(text, "watchlist=") != NULL);

    umi_trader_services_destroy(&services);
    return 0;
}
