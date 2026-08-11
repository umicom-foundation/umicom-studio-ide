/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/tests/test_risk_dashboard.c
 *
 * PURPOSE:
 *   Validate the Umicom Trader risk dashboard foundation.
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
int main(void){UmiTraderServices s;assert(umi_trader_services_init(&s)==UMI_STATUS_OK);UmiTraderRiskSnapshot r=umi_trader_risk_snapshot(&s);assert(r.max_order_quantity==10.0);umi_trader_services_destroy(&s);return 0;}
