/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/order_ticket.c
 *
 * PURPOSE:
 *   Submit a Trader order through OMS risk controls and the configured broker.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/order_ticket.h"

UmiStatus umi_trader_order_submit(UmiTraderServices *services,
                                  const UmiOrderRequest *request,
                                  double current_position,
                                  double daily_pnl,
                                  int64_t now_ms,
                                  UmiRiskDecision *decision)
{
    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const UmiStatus status =
        umi_oms_submit(&services->oms,
                       request,
                       current_position,
                       daily_pnl,
                       now_ms,
                       decision);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return services->broker.submit_order(services->broker.instance,
                                         request);
}
