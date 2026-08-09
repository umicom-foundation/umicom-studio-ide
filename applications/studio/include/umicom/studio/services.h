/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/services.h
 *
 * PURPOSE:
 *   Own the Umicom Framework services shared by Studio frontends and Slave
 *   Controllers.  The container creates one diagnostic hub, one bounded
 *   retained diagnostic store, one typed settings repository, and one
 *   Framework clock so features do not create uncontrolled global services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SERVICES_H
#define UMICOM_STUDIO_SERVICES_H

#include <stddef.h>

#include "umicom/umicom.h"
#include "umicom/studio/settings.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioServices UmiStudioServices;

UmiStatus umi_studio_services_create(
    UmiDiagnosticSink initial_sink,
    void *initial_user_data,
    UmiStudioServices **out_services
);

void umi_studio_services_destroy(UmiStudioServices *services);

UmiStatus umi_studio_services_add_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data
);

UmiStatus umi_studio_services_remove_diagnostic_sink(
    UmiStudioServices *services,
    UmiDiagnosticSink sink,
    void *user_data
);

UmiDiagnosticSink umi_studio_services_diagnostic_sink(void);
void *umi_studio_services_diagnostic_user_data(UmiStudioServices *services);
UmiClock *umi_studio_services_clock(UmiStudioServices *services);
UmiSettings *umi_studio_services_settings(UmiStudioServices *services);
UmiDiagnosticStore *umi_studio_services_diagnostic_store(
    UmiStudioServices *services
);

size_t umi_studio_services_diagnostic_sink_count(
    const UmiStudioServices *services
);

#ifdef __cplusplus
}
#endif

#endif
