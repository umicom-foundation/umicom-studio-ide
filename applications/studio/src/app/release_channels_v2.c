/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/release_channels_v2.c
 *
 * PURPOSE:
 *   Own the Studio development, beta and stable update channel catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/release_channels_v2.h"
#include <string.h>

UmiStatus umi_studio_release_channels_v2_init(
    UmiStudioReleaseChannelsV2 *channels)
{
    UmiStatus status;
    if (channels == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(channels, 0, sizeof(*channels));
    status = umi_update_channel_v2_init(
        &channels->channels[0], "stable",
        "https://updates.umicom.org/studio/stable.json",
        UMI_RELEASE_STABLE, 100U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_update_channel_v2_init(
        &channels->channels[1], "beta",
        "https://updates.umicom.org/studio/beta.json",
        UMI_RELEASE_BETA, 50U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_update_channel_v2_init(
        &channels->channels[2], "development",
        "https://updates.umicom.org/studio/development.json",
        UMI_RELEASE_DEVELOPMENT, 100U);
    if (status != UMI_STATUS_OK) return status;
    channels->count = UMI_STUDIO_MAX_RELEASE_CHANNELS_V2;
    channels->selected = 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_release_channels_v2_select(
    UmiStudioReleaseChannelsV2 *channels,
    const char *channel_id)
{
    size_t index;
    if (channels == NULL || channel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < channels->count; ++index) {
        if (strcmp(channels->channels[index].channel_id, channel_id) == 0) {
            channels->selected = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiUpdateChannelV2 *umi_studio_release_channels_v2_current(
    const UmiStudioReleaseChannelsV2 *channels)
{
    if (channels == NULL || channels->selected >= channels->count) return NULL;
    return &channels->channels[channels->selected];
}
