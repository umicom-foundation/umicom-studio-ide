/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/build_workbench.h
 *
 * PURPOSE:
 *   Project Framework build results into Studio Problems and named Output
 *   channels without placing UI knowledge inside the reusable build engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_BUILD_WORKBENCH_H
#define UMICOM_STUDIO_BUILD_WORKBENCH_H

#include "umicom/build/result.h"
#include "umicom/diagnostic_ui/diagnostic_ui.h"
#include "umicom/ui/output_channel.h"
#include "umicom/ui/problem.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_build_publish_result(
    const UmiBuildResult *result,
    UmiUiProblemRegistry *problems,
    UmiUiOutputChannelRegistry *output_channels);

#ifdef __cplusplus
}
#endif
#endif
