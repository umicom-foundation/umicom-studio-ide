/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_replay_lab.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader replay lab foundation.
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

int main(void)
{
    UmiTraderReplayLab lab;
    umi_trader_replay_lab_init(&lab, 1U, 0);

    UmiReplayEvent event = {1U, 100, {0}, {0}};
    (void)snprintf(event.type,
                   sizeof(event.type),
                   "%s",
                   "quote");

    assert(umi_trader_replay_lab_accept(&lab, &event));
    assert(lab.accepted_events == 1U);
    return 0;
}
