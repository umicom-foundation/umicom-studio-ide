/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/risk_dashboard.c
 *
 * PURPOSE:
 *   Produce a compact risk-readiness snapshot for Trader UI adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/risk_dashboard.h"

UmiTraderRiskSnapshot
umi_trader_risk_snapshot(const UmiTraderServices *services)
{
    UmiTraderRiskSnapshot snapshot = {0};

    if (services != NULL) {
        snapshot.kill_switch_engaged =
            services->oms.kill_switch.engaged;
        snapshot.max_order_quantity =
            services->oms.risk_limit.max_order_quantity;
        snapshot.max_position_quantity =
            services->oms.risk_limit.max_position_quantity;
        snapshot.max_daily_loss =
            services->oms.risk_limit.max_daily_loss;
    }

    return snapshot;
}
