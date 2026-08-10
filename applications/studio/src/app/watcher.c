/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/watcher.c
 *
 * PURPOSE:
 *   Provide Studio-level lifecycle and inspection functions for the shared
 *   Framework workspace watcher.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/watcher.h"

UmiStatus umi_studio_watcher_start(UmiStudioServices *services)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_watcher_start(umi_studio_services_watcher(services));
}

UmiStatus umi_studio_watcher_stop(UmiStudioServices *services)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_watcher_stop(umi_studio_services_watcher(services));
}

UmiStatus umi_studio_watcher_scan_once(UmiStudioServices *services)
{
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_watcher_scan_once(umi_studio_services_watcher(services));
}

UmiWatcherStats umi_studio_watcher_stats(UmiStudioServices *services)
{
    return services != NULL
        ? umi_watcher_stats(umi_studio_services_watcher(services))
        : (UmiWatcherStats){0};
}
