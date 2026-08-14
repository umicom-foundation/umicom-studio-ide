/* Umicom Studio IDE Tests | Batch 57 fixtures | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_TEST_OPERATIONS_WORKSPACE_V2_FIXTURE_H
#define UMICOM_STUDIO_TEST_OPERATIONS_WORKSPACE_V2_FIXTURE_H
#include <assert.h>
#include "umicom/studio/operations_commands_v2.h"
#include "umicom/studio/workspace_commands_v2.h"
static inline UmiStudioServices *umi_test_batch57_services(void)
{
    static int services_token;
    return (UmiStudioServices *)(void *)&services_token;
}
static inline UmiStudioOperationsCentreV2 *umi_test_operations_centre_v2(void)
{
    UmiStudioOperationsCentreV2 *centre = NULL;
    assert(umi_studio_operations_centre_v2_create(umi_test_batch57_services(),&centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioOperationsCentreV2 *umi_test_seeded_operations_centre_v2(void)
{
    UmiStudioOperationsCentreV2 *centre = umi_test_operations_centre_v2();
    assert(umi_studio_operations_seed_v2(centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioProfessionalWorkspaceV2 *umi_test_professional_workspace_v2(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = NULL;
    assert(umi_studio_professional_workspace_v2_create(umi_test_batch57_services(),&workspace) == UMI_STATUS_OK);
    return workspace;
}
static inline UmiStudioProfessionalWorkspaceV2 *umi_test_seeded_professional_workspace_v2(void)
{
    UmiStudioProfessionalWorkspaceV2 *workspace = umi_test_professional_workspace_v2();
    assert(umi_studio_workspace_seed_v2(workspace) == UMI_STATUS_OK);
    return workspace;
}
#endif
