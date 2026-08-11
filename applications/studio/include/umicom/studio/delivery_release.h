/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/delivery_release.h
 *
 * PURPOSE:
 *   Prepare Studio release candidates and register verified local release records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These helpers translate Studio product identity into Framework release records without implementing generic release mechanics twice.
 */

#ifndef APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_RELEASE_H
#define APPLICATIONS_STUDIO_INCLUDE_UMICOM_STUDIO_DELIVERY_RELEASE_H

#include "umicom/umicom.h"
#include "umicom/studio/delivery_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_studio_release_prepare(UmiStudioDeliveryPlatform *platform,
                                      const char *release_id,
                                      const char *source_revision,
                                      UmiReleaseChannel channel,
                                      UmiReleaseCandidate *out_candidate);
UmiStatus umi_studio_release_register(UmiStudioDeliveryPlatform *platform,
                                      const UmiReleaseCandidate *candidate,
                                      uint64_t generation);

#ifdef __cplusplus
}
#endif

#endif
