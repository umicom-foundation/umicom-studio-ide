/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/plugins.h
 *
 * PURPOSE:
 *   Provide Studio plug-in discovery and reporting as a thin product adapter
 *   over the Framework plug-in host.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_PLUGINS_H
#define UMICOM_STUDIO_PLUGINS_H

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioPluginReport {
    size_t registered;
    size_t enabled;
    size_t failed;
    size_t contributions;
    size_t files_examined;
    size_t manifests_loaded;
    size_t invalid_manifests;
    size_t extension_points;
    size_t catalogue_entries;
    size_t audit_events;
    size_t permission_grants;
    char default_isolation[64];
} UmiStudioPluginReport;

UmiStatus umi_studio_plugins_discover(UmiStudioServices *services,
                                      const char *root,
                                      int recursive,
                                      UmiStudioPluginReport *out_report);
UmiStatus umi_studio_plugins_report(UmiStudioServices *services,
                                    UmiStudioPluginReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
