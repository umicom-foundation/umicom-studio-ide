/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/broker_settings.h
 *
 * PURPOSE:
 *   Build safe default simulation/paper broker connection settings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework broker settings capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_BROKER_SETTINGS_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_BROKER_SETTINGS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiIbkrConnectionSettings umi_trader_default_ibkr_settings(UmiTradingEnvironment environment);
#ifdef __cplusplus
}
#endif
#endif
