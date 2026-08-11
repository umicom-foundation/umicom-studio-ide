/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/cli/risk_main.c
 *
 * PURPOSE:
 *   Provide a native headless Umicom Trader foundation command.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These small executables prove the product can run without a GUI and exercise the same services future GTK and web frontends will use.
 */

#include <stdio.h>
#include "umicom/trader/trader.h"

int main(void)
{
    UmiTraderServices services;
    if (umi_trader_services_init(&services) != UMI_STATUS_OK) {
        return 1;
    }

    const UmiTraderRiskSnapshot risk =
        umi_trader_risk_snapshot(&services);

    printf("kill_switch=%d max_order=%.2f "
           "max_position=%.2f max_daily_loss=%.2f\n",
           risk.kill_switch_engaged,
           risk.max_order_quantity,
           risk.max_position_quantity,
           risk.max_daily_loss);

    umi_trader_services_destroy(&services);
    return 0;
}
