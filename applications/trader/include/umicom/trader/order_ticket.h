/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/order_ticket.h
 *
 * PURPOSE:
 *   Submit a Trader order through OMS risk controls and the configured broker.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework order ticket capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_ORDER_TICKET_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_ORDER_TICKET_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/services.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_trader_order_submit(UmiTraderServices *services, const UmiOrderRequest *request, double current_position, double daily_pnl, int64_t now_ms, UmiRiskDecision *decision);
#ifdef __cplusplus
}
#endif
#endif
