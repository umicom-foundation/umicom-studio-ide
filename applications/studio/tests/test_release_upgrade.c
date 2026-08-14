/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_release_upgrade.c
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
#include "umicom/studio/release_channels.h"
#include "umicom/studio/upgrade_centre.h"

int main(void)
{
    UmiStudioReleaseChannels channels;
    UmiStudioUpgradeCentre upgrade;
    assert(umi_studio_release_channels_init(&channels) == UMI_STATUS_OK);
    assert(umi_studio_release_channels_select(&channels, "beta") ==
           UMI_STATUS_OK);
    assert(strcmp(umi_studio_release_channels_current(&channels)->channel_id,
                  "beta") == 0);
    assert(umi_studio_upgrade_centre_prepare(
               &upgrade, "0.22.0", "0.23.0", 58U, 59U, 1) ==
           UMI_STATUS_OK);
    assert(umi_studio_upgrade_centre_approve(&upgrade, 1) == UMI_STATUS_OK);
    assert(umi_studio_upgrade_centre_ready(&upgrade));
    return 0;
}
