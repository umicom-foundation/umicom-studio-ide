/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/risk_dashboard.h
 *
 * PURPOSE:
 *   Produce a compact risk-readiness snapshot for Trader UI adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework risk dashboard capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RISK_DASHBOARD_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RISK_DASHBOARD_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderRiskSnapshot { int kill_switch_engaged; double max_order_quantity; double max_position_quantity; double max_daily_loss; } UmiTraderRiskSnapshot;
UmiTraderRiskSnapshot umi_trader_risk_snapshot(const UmiTraderServices *services);
#ifdef __cplusplus
}
#endif
#endif
