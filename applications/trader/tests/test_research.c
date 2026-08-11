/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_research.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader research foundation.
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
int main(void){UmiTraderResearchLab l;umi_trader_research_init(&l);UmiMovementEvent e=umi_trader_research_record_move(&l,30000,30110,0,1800000);assert(e.qualifies);assert(l.stats.qualifying_moves==1U);return 0;}
