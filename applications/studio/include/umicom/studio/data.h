/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/data.h
 *
 * PURPOSE:
 *   Declare Studio use cases over the authoritative Framework Data Server, including key/value state, transactions, integrity reporting and schema migration planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_DATA_H
#define UMICOM_STUDIO_DATA_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/studio/services.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiStudioDataReport {
    UmiDataServerBackend backend;
    char backend_name[32];
    char path[UMI_PATH_CAPACITY];
    size_t records;
    uint32_t schema_version;
    int transaction_active;
    UmiStatus integrity_status;
} UmiStudioDataReport;

UmiStatus umi_studio_data_set(UmiStudioServices *services,
                              const char *key,
                              const char *value);
UmiStatus umi_studio_data_get(UmiStudioServices *services,
                              const char *key,
                              char *value,
                              size_t value_capacity);
UmiStatus umi_studio_data_remove(UmiStudioServices *services,
                                 const char *key);
UmiStatus umi_studio_data_report(UmiStudioServices *services,
                                 UmiStudioDataReport *out_report);
UmiStatus umi_studio_data_transaction_set(UmiStudioServices *services,
                                          const char *key,
                                          const char *value);
UmiStatus umi_studio_data_apply_core_migrations(UmiStudioServices *services,
                                                uint32_t target_version,
                                                size_t *out_applied);

#ifdef __cplusplus
}
#endif

#endif
