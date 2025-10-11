/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/watcher_integration.h
 * PURPOSE: Glue recursive watcher to file tree refresh
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WATCHER_INTEGRATION_H
#define UMICOM_WATCHER_INTEGRATION_H

#include "watcher_recursive.h"
#include "../../fs/include/file_tree.h"
#include "../../../core/include/workspace.h"

typedef struct {
  UmiWatchRec *rec;
  UmiFileTree *tree;
  UmiWorkspace *ws;
} UmiWatchGlue;

UmiWatchGlue *umi_watchglue_start(UmiWorkspace *ws, UmiFileTree *tree);
void          umi_watchglue_stop(UmiWatchGlue *g);

#endif /* UMICOM_WATCHER_INTEGRATION_H */
