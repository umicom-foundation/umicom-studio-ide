/*-----------------------------------------------------------------------------
 * Umicom Trader
 * File: applications/trader/src/app/replay_lab.c
 *
 * PURPOSE:
 *   Represent Trader historical replay state using the shared replay cursor and clock.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file contains only Trader composition or product policy; reusable trading mechanics remain in Umicom Framework.
 */

#include "umicom/trader/replay_lab.h"
void umi_trader_replay_lab_init(UmiTraderReplayLab *l,uint64_t first,int64_t start){if(l!=NULL){umi_replay_cursor_init(&l->cursor,first);umi_replay_clock_init(&l->clock,start,1.0);l->accepted_events=0U;}}
int umi_trader_replay_lab_accept(UmiTraderReplayLab *l,const UmiReplayEvent *e){if(l==NULL||!umi_replay_cursor_accept(&l->cursor,e))return 0;umi_replay_clock_advance(&l->clock,e->event_time_ms);l->accepted_events++;return 1;}
