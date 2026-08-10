/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/state.h
 *
 * PURPOSE:
 *   Define an immutable operational snapshot of Studio's Framework modules,
 *   services, commands, health, tasks, documents and session state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_STATE_H
#define UMICOM_STUDIO_STATE_H

#include <stddef.h>

#include "umicom/studio/bootstrap.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioStateReport {
    size_t module_count;
    size_t service_count;
    size_t command_count;
    size_t health_count;
    UmiHealthState overall_health;
    UmiTaskQueueStats tasks;
    size_t document_count;
    size_t session_entry_count;
} UmiStudioStateReport;

UmiStatus umi_studio_state_capture(UmiStudioBootstrap *bootstrap,
                                   UmiStudioStateReport *out_report);
UmiStatus umi_studio_state_format(const UmiStudioStateReport *report,
                                  char *out_text,
                                  size_t text_capacity);

#ifdef __cplusplus
}
#endif

#endif
