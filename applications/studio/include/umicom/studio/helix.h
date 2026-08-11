/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix.h
 *
 * PURPOSE:
 *   Expose Studio operations over the shared Framework Helix runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio displays and advances a governed Framework workflow; it does not let the GUI mutate Helix internals directly.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_status(UmiStudioAiPlatform *platform,
                                  char *output,
                                  size_t output_capacity);
UmiStatus umi_studio_helix_advance(UmiStudioAiPlatform *platform,
                                   UmiHelixWorkflowStage next_stage);

#ifdef __cplusplus
}
#endif

#endif
