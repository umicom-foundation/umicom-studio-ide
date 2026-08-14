/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/release_channels.h
 *
 * PURPOSE:
 *   Own the Studio development, beta and stable update channel catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RELEASE_CHANNELS_H
#define UMICOM_STUDIO_RELEASE_CHANNELS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_MAX_RELEASE_CHANNELS 3U

typedef struct UmiStudioReleaseChannels {
    UmiUpdateChannel channels[UMI_STUDIO_MAX_RELEASE_CHANNELS];
    size_t count;
    size_t selected;
} UmiStudioReleaseChannels;

UmiStatus umi_studio_release_channels_init(
    UmiStudioReleaseChannels *channels);
UmiStatus umi_studio_release_channels_select(
    UmiStudioReleaseChannels *channels,
    const char *channel_id);
const UmiUpdateChannel *umi_studio_release_channels_current(
    const UmiStudioReleaseChannels *channels);

#ifdef __cplusplus
}
#endif
#endif
