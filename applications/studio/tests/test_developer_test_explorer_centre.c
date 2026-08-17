/*-----------------------------------------------------------------------------
 * Umicom Studio IDE professional Test Explorer contribution tests.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/studio/test_explorer_centre.h"

int main(void)
{
    UmiStudioTestExplorerCentre *centre = NULL;
    UmiStudioTestExplorerCentreSnapshot snapshot;
    size_t index;

    assert(umi_studio_test_explorer_centre_create(&centre) == UMI_STATUS_OK);
    assert(umi_studio_test_explorer_centre_snapshot(centre, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.available);
    assert(snapshot.api_version == UMI_STUDIO_TEST_EXPLORER_CENTRE_API_VERSION);
    assert(snapshot.command_contribution_count ==
           umi_test_platform_command_count());
    assert(snapshot.view_contribution_count == 8U);
    assert(snapshot.experience.provider_count >= 2U);

    for (index = 0U;
         index < umi_studio_test_explorer_command_contribution_count();
         ++index) {
        const UmiStudioTestExplorerCommandContribution *contribution =
            umi_studio_test_explorer_command_contribution_at(index);
        size_t comparison;
        assert(contribution != NULL);
        assert(contribution->struct_size == (uint32_t)sizeof(*contribution));
        assert(umi_test_platform_command_find(
                   contribution->framework_command_id) != NULL);
        assert(contribution->show_in_command_centre);
        for (comparison = index + 1U;
             comparison <
                 umi_studio_test_explorer_command_contribution_count();
             ++comparison) {
            assert(strcmp(
                       contribution->framework_command_id,
                       umi_studio_test_explorer_command_contribution_at(
                           comparison)->framework_command_id) != 0);
        }
    }
    assert(umi_studio_test_explorer_command_contribution_find(
               "test.coverage-all") != NULL);
    assert(umi_studio_test_explorer_command_contribution_find(
               "test.missing") == NULL);
    assert(umi_studio_test_explorer_view_contribution_find(
               "studio.test.coverage") != NULL);
    assert(umi_studio_test_explorer_view_contribution_find(
               "studio.test.rerun-planner") != NULL);
    assert(umi_studio_test_explorer_view_contribution_find(
               "studio.test.missing") == NULL);

    umi_studio_test_explorer_centre_destroy(centre);
    return 0;
}
