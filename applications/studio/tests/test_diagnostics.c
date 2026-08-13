/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_diagnostics.c
 *
 * PURPOSE:
 *   Verify Studio's diagnostic adapter can emit records through the Framework
 *   hub, read them in chronological order, report summary counters, and clear
 *   the retained store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/diagnostics.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiStudioServices *services = NULL;
    UmiDiagnosticStoreSummary summary;
    UmiDiagnosticRecord record;

    assert(umi_studio_services_create(NULL, NULL, &services) == UMI_STATUS_OK);
    assert(services != NULL);
    assert(umi_studio_services_diagnostic_sink_count(services) == 2U);
    assert(umi_studio_services_diagnostic_pipeline(services) != NULL);

    assert(umi_studio_diagnostics_emit(services,
                                       UMI_DIAGNOSTIC_INFO,
                                       "studio-diagnostics-test",
                                       "first",
                                       101U) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_emit(services,
                                       UMI_DIAGNOSTIC_WARNING,
                                       "studio-diagnostics-test",
                                       "second",
                                       102U) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_emit(services,
                                       UMI_DIAGNOSTIC_ERROR,
                                       "studio-diagnostics-test",
                                       "third",
                                       103U) == UMI_STATUS_OK);

    assert(umi_studio_diagnostics_summary(services, &summary) == UMI_STATUS_OK);
    assert(summary.retained_count == 3U);
    assert(summary.total_received == 3U);
    assert(summary.retained_by_severity[1] == 1U);
    assert(summary.retained_by_severity[2] == 1U);
    assert(summary.retained_by_severity[3] == 1U);

    assert(umi_studio_diagnostics_record_at(services,
                                            1U,
                                            &record) == UMI_STATUS_OK);
    assert(record.severity == UMI_DIAGNOSTIC_WARNING);
    assert(record.correlation_id == 102U);
    assert(strcmp(record.message, "second") == 0);

    assert(umi_studio_diagnostics_clear(services) == UMI_STATUS_OK);
    assert(umi_studio_diagnostics_count(services) == 0U);

    umi_studio_services_destroy(services);
    return 0;
}
