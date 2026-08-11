/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/delivery_report.c
 *
 * PURPOSE:
 *   Build a compact Studio release report from Framework candidate and pipeline state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The same structured report can be shown in a future GTK Release perspective or printed by native tools.
 */

#include "umicom/studio/delivery_report.h"
void umi_delivery_report_init(UmiDeliveryReport *report);

UmiStatus umi_studio_delivery_report(const UmiReleaseCandidate *candidate,
                                     const UmiDeliveryPipeline *pipeline,
                                     UmiDeliveryReport *out_report)
{
    if (candidate == NULL || pipeline == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_delivery_report_init(out_report);
    out_report->artifacts = candidate->artifacts.count;
    out_report->gates = pipeline->gate_count;
    out_report->failed_gates = umi_delivery_pipeline_failed_count(pipeline);
    out_report->tests_passed = candidate->build.tests_passed;
    out_report->tests_failed = candidate->build.tests_failed;
    out_report->ready = umi_release_candidate_ready(candidate) &&
                        umi_delivery_pipeline_passed(pipeline);
    return UMI_STATUS_OK;
}
