/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/commands.h
 *
 * PURPOSE:
 *   Publish the initial Trader command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework commands capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_COMMANDS_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_COMMANDS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_trader_command_count(void);
const char *umi_trader_command_at(size_t index);
#ifdef __cplusplus
}
#endif
#endif
