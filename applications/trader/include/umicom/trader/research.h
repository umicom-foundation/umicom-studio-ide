/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/research.h
 *
 * PURPOSE:
 *   Track >=100-point movement research and factor observations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework research capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RESEARCH_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RESEARCH_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTraderResearchLab { UmiResearchStats stats; UmiFeatureVector factors; double movement_threshold_points; int64_t movement_window_ms; } UmiTraderResearchLab;
void umi_trader_research_init(UmiTraderResearchLab *lab);
UmiMovementEvent umi_trader_research_record_move(UmiTraderResearchLab *lab, double start_price, double end_price, int64_t start_ms, int64_t end_ms);
#ifdef __cplusplus
}
#endif
#endif
