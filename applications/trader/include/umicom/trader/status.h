/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/status.h
 *
 * PURPOSE:
 *   Render a concise text status for CLI and future workbench adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework status capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_STATUS_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_STATUS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/services.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_trader_status_text(const UmiTraderServices *services, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
