/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_release_upgrade_v2.c
 *
 * PURPOSE:
 *   Verify Studio release-channel selection and rollback-safe upgrades.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio/release_channels_v2.h"
#include "umicom/studio/upgrade_centre_v2.h"

int main(void)
{
    UmiStudioReleaseChannelsV2 channels;
    UmiStudioUpgradeCentreV2 upgrade;
    assert(umi_studio_release_channels_v2_init(&channels) == UMI_STATUS_OK);
    assert(umi_studio_release_channels_v2_select(&channels, "beta") ==
           UMI_STATUS_OK);
    assert(strcmp(umi_studio_release_channels_v2_current(&channels)->channel_id,
                  "beta") == 0);
    assert(umi_studio_upgrade_centre_v2_prepare(
               &upgrade, "0.22.0", "0.23.0", 58U, 59U, 1) ==
           UMI_STATUS_OK);
    assert(umi_studio_upgrade_centre_v2_approve(&upgrade, 1) == UMI_STATUS_OK);
    assert(umi_studio_upgrade_centre_v2_ready(&upgrade));
    return 0;
}
