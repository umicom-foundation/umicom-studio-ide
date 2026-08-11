/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/helix_evidence.h
 *
 * PURPOSE:
 *   Record compact Helix evidence in the shared evolutionary memory.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio writes evidence through the Helix memory API so later durable storage can replace the in-memory reference implementation transparently.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_EVIDENCE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_HELIX_EVIDENCE_H

#include "umicom/studio/ai_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_helix_record_evidence(UmiStudioAiPlatform *platform,
                                           const char *key,
                                           const char *summary);
const char *umi_studio_helix_evidence(UmiStudioAiPlatform *platform,
                                      const char *key);

#ifdef __cplusplus
}
#endif

#endif
