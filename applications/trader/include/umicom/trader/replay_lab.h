/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/replay_lab.h
 *
 * PURPOSE:
 *   Represent Trader historical replay state using the shared replay cursor and clock.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework replay lab capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_REPLAY_LAB_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_REPLAY_LAB_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderReplayLab { UmiReplayCursor cursor; UmiReplayClock clock; uint64_t accepted_events; } UmiTraderReplayLab;
void umi_trader_replay_lab_init(UmiTraderReplayLab *lab, uint64_t first_sequence, int64_t start_ms);
int umi_trader_replay_lab_accept(UmiTraderReplayLab *lab, const UmiReplayEvent *event);
#ifdef __cplusplus
}
#endif
#endif
