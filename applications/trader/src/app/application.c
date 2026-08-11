/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/application.c
 *
 * PURPOSE:
 *   Provide the headless Umicom Trader product lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/application.h"
UmiStatus umi_trader_application_start(UmiTraderApplication *a){if(a==NULL)return UMI_STATUS_INVALID_ARGUMENT;UmiStatus s=umi_trader_services_init(&a->services);if(s==UMI_STATUS_OK)a->started=1;return s;}
void umi_trader_application_stop(UmiTraderApplication *a){if(a!=NULL){umi_trader_services_destroy(&a->services);a->started=0;}}
