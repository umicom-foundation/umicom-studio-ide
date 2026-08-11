/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/application.h
 *
 * PURPOSE:
 *   Provide the headless Umicom Trader product lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework application capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_APPLICATION_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_APPLICATION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/services.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderApplication { UmiTraderServices services; int started; } UmiTraderApplication;
UmiStatus umi_trader_application_start(UmiTraderApplication *application);
void umi_trader_application_stop(UmiTraderApplication *application);
#ifdef __cplusplus
}
#endif
#endif
