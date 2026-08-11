/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_application.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader application foundation.
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
int main(void){UmiTraderApplication a={0};assert(umi_trader_application_start(&a)==UMI_STATUS_OK);assert(a.started);umi_trader_application_stop(&a);assert(!a.started);return 0;}
