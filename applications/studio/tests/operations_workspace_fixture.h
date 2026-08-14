/* Umicom Studio IDE Tests | Batch 57 fixtures | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_STUDIO_TEST_OPERATIONS_WORKSPACE_FIXTURE_H
#define UMICOM_STUDIO_TEST_OPERATIONS_WORKSPACE_FIXTURE_H
#include <assert.h>
#include "umicom/studio/operations_commands.h"
#include "umicom/studio/workspace_commands.h"
static inline UmiStudioServices *umi_test_batch57_services(void)
{
    static int services_token;
    return (UmiStudioServices *)(void *)&services_token;
}
static inline UmiStudioOperationsCentre *umi_test_operations_centre(void)
{
    UmiStudioOperationsCentre *centre = NULL;
    assert(umi_studio_operations_centre_create(umi_test_batch57_services(),&centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioOperationsCentre *umi_test_seeded_operations_centre(void)
{
    UmiStudioOperationsCentre *centre = umi_test_operations_centre();
    assert(umi_studio_operations_seed(centre) == UMI_STATUS_OK);
    return centre;
}
static inline UmiStudioProfessionalWorkspace *umi_test_professional_workspace(void)
{
    UmiStudioProfessionalWorkspace *workspace = NULL;
    assert(umi_studio_professional_workspace_create(umi_test_batch57_services(),&workspace) == UMI_STATUS_OK);
    return workspace;
}
static inline UmiStudioProfessionalWorkspace *umi_test_seeded_professional_workspace(void)
{
    UmiStudioProfessionalWorkspace *workspace = umi_test_professional_workspace();
    assert(umi_studio_workspace_seed(workspace) == UMI_STATUS_OK);
    return workspace;
}
#endif
