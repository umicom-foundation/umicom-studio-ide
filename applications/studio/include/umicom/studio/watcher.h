/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/watcher.h
 *
 * PURPOSE:
 *   Control Studio's shared Framework watcher and expose watcher statistics to
 *   headless tools, tests and future GTK4 workbench surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_WATCHER_H
#define UMICOM_STUDIO_WATCHER_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_watcher_start(UmiStudioServices *services);
UmiStatus umi_studio_watcher_stop(UmiStudioServices *services);
UmiStatus umi_studio_watcher_scan_once(UmiStudioServices *services);
UmiWatcherStats umi_studio_watcher_stats(UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
