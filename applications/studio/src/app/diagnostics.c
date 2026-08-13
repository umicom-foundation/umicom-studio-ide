/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/diagnostics.c
 *
 * PURPOSE:
 *   Implement the Studio-facing diagnostic API as a narrow adapter over the
 *   Framework diagnostic hub and bounded diagnostic store owned by
 *   UmiStudioServices.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/diagnostics.h"

UmiStatus umi_studio_diagnostics_emit(
    UmiStudioServices *services,
    UmiDiagnosticSeverity severity,
    const char *source,
    const char *message,
    uint64_t correlation_id)
{
    UmiDiagnosticSink sink;
    void *user_data;

    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    sink = umi_studio_services_diagnostic_sink();
    user_data = umi_studio_services_diagnostic_user_data(services);
    if (sink == NULL || user_data == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    umi_diagnostic_emit(sink,
                        user_data,
                        severity,
                        source,
                        message,
                        correlation_id);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_diagnostics_summary(
    UmiStudioServices *services,
    UmiDiagnosticStoreSummary *out_summary)
{
    UmiDiagnosticStore *store;

    if (services == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store = umi_studio_services_diagnostic_store(services);
    if (store == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_diagnostic_store_summary(store, out_summary);
}

size_t umi_studio_diagnostics_count(UmiStudioServices *services)
{
    UmiDiagnosticStore *store =
        umi_studio_services_diagnostic_store(services);
    return store != NULL ? umi_diagnostic_store_count(store) : 0U;
}

UmiStatus umi_studio_diagnostics_record_at(
    UmiStudioServices *services,
    size_t chronological_index,
    UmiDiagnosticRecord *out_record)
{
    UmiDiagnosticStore *store;

    if (services == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store = umi_studio_services_diagnostic_store(services);
    if (store == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_diagnostic_store_record_at(store,
                                          chronological_index,
                                          out_record);
}

UmiStatus umi_studio_diagnostics_clear(UmiStudioServices *services)
{
    UmiDiagnosticStore *store;

    if (services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    store = umi_studio_services_diagnostic_store(services);
    if (store == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    umi_diagnostic_store_clear(store);
    umi_diagnostic_pipeline_clear(
        umi_studio_services_diagnostic_pipeline(services));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_diagnostics_clear_problems(UmiStudioServices *services)
{
    UmiDiagnosticPipeline *pipeline;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pipeline = umi_studio_services_diagnostic_pipeline(services);
    if (pipeline == NULL) return UMI_STATUS_INVALID_STATE;
    umi_diagnostic_store_clear(umi_studio_services_diagnostic_store(services));
    umi_diagnostic_model_clear(umi_diagnostic_pipeline_model(pipeline));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_diagnostics_clear_output(UmiStudioServices *services)
{
    UmiDiagnosticPipeline *pipeline;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pipeline = umi_studio_services_diagnostic_pipeline(services);
    if (pipeline == NULL) return UMI_STATUS_INVALID_STATE;
    umi_output_buffer_clear(umi_diagnostic_pipeline_output(pipeline));
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_diagnostics_pipeline_snapshot(
    UmiStudioServices *services,
    UmiDiagnosticPipelineSnapshot *out_snapshot)
{
    if (services == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_diagnostic_pipeline_snapshot(
        umi_studio_services_diagnostic_pipeline(services), out_snapshot);
}

UmiStatus umi_studio_diagnostics_ingest_build_result(
    UmiStudioServices *services,
    const UmiBuildResult *result)
{
    if (services == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_diagnostic_build_result_ingest(
        umi_studio_services_diagnostic_pipeline(services), result, "Umicom Build");
}
