/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/include/umicom/trader/research_report.h
 *
 * PURPOSE:
 *   Render the movement-research summary used by the historical study workflow.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Trader-facing API composes shared Framework research report capabilities without copying the underlying mechanism into the product.
 */

#ifndef APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RESEARCH_REPORT_H
#define APPLICATIONS_TRADER_INCLUDE_UMICOM_TRADER_RESEARCH_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/trading/trading.h"
#include "umicom/trader/research.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_trader_research_report(const UmiTraderResearchLab *lab, char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
