/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/release_channels_v2.h
 *
 * PURPOSE:
 *   Own the Studio development, beta and stable update channel catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RELEASE_CHANNELS_V2_H
#define UMICOM_STUDIO_RELEASE_CHANNELS_V2_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_MAX_RELEASE_CHANNELS_V2 3U

typedef struct UmiStudioReleaseChannelsV2 {
    UmiUpdateChannelV2 channels[UMI_STUDIO_MAX_RELEASE_CHANNELS_V2];
    size_t count;
    size_t selected;
} UmiStudioReleaseChannelsV2;

UmiStatus umi_studio_release_channels_v2_init(
    UmiStudioReleaseChannelsV2 *channels);
UmiStatus umi_studio_release_channels_v2_select(
    UmiStudioReleaseChannelsV2 *channels,
    const char *channel_id);
const UmiUpdateChannelV2 *umi_studio_release_channels_v2_current(
    const UmiStudioReleaseChannelsV2 *channels);

#ifdef __cplusplus
}
#endif
#endif
