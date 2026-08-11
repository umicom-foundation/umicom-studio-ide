/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_commands.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader commands foundation.
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
int main(void){assert(umi_trader_command_count()==8U);assert(strcmp(umi_trader_command_at(0),"trader.market.watch")==0);return 0;}
