/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/delivery_report.h
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

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_REPORT_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_REPORT_H

#include "umicom/umicom.h"
#include "umicom/studio/delivery_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_delivery_report(const UmiReleaseCandidate *candidate,
                                        const UmiDeliveryPipeline *pipeline,
                                        UmiDeliveryReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
