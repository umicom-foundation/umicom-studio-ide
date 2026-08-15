/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/trading.h
 *
 * PURPOSE:
 *   Compose Studio's safe reference trading workspace from reusable Framework
 *   services while keeping broker connectivity and live execution outside the
 *   IDE product boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns only the lifetime and deterministic demonstration market. The
 * trading rules, risk checks and view projections remain Framework-owned so a
 * dedicated Trader or TMS application can reuse them unchanged.
 */
#ifndef UMICOM_STUDIO_TRADING_H
#define UMICOM_STUDIO_TRADING_H

#include "umicom/trading/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioTradingService UmiStudioTradingService;

UmiStatus umi_studio_trading_service_create(
    UmiStudioTradingService **out_service);
void umi_studio_trading_service_destroy(UmiStudioTradingService *service);
UmiTradingWorkspace *umi_studio_trading_service_workspace(
    UmiStudioTradingService *service);
UmiStatus umi_studio_trading_service_snapshot(
    UmiStudioTradingService *service,
    UmiTradingWorkspaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
