/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/services.h
 *
 * PURPOSE:
 *   Own the Framework services shared by Studio frontends and Slave
 *   Controllers.  This creates one controlled place for diagnostics and time
 *   instead of allowing each feature to create private global services.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_SERVICES_H
#define UMICOM_STUDIO_SERVICES_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioServices UmiStudioServices;

UmiStatus umi_studio_services_create(UmiDiagnosticSink initial_sink,
                                     void *initial_user_data,
                                     UmiStudioServices **out_services);
void umi_studio_services_destroy(UmiStudioServices *services);
UmiStatus umi_studio_services_add_diagnostic_sink(UmiStudioServices *services,
                                                  UmiDiagnosticSink sink,
                                                  void *user_data);
UmiDiagnosticSink umi_studio_services_diagnostic_sink(void);
void *umi_studio_services_diagnostic_user_data(UmiStudioServices *services);
UmiClock *umi_studio_services_clock(UmiStudioServices *services);
size_t umi_studio_services_diagnostic_sink_count(const UmiStudioServices *services);

#ifdef __cplusplus
}
#endif

#endif
