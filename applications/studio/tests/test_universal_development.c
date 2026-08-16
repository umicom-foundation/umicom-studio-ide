/* Umicom Studio thin universal-development composition regression test.
 * All executable language, project and toolchain logic remains in Framework.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include <assert.h>

#include "umicom/studio/developer_workbench.h"

int main(void)
{
    UmiStudioDeveloperWorkbench *workbench = NULL;
    UmiDeveloperUniversalModel *model;
    UmiDeveloperUniversalModelSnapshot snapshot;

    assert(umi_studio_developer_workbench_create(&workbench) == UMI_STATUS_OK);
    model = umi_studio_developer_workbench_universal_model(workbench);
    assert(model != NULL);
    assert(model == umi_developer_runtime_universal_model(
                        umi_studio_developer_workbench_runtime(workbench)));
    assert(umi_developer_universal_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.languages.profile_count >= 20U);
    assert(snapshot.toolchains.binding_count >= 20U);
    assert(umi_developer_universal_model_register_project(
               model, "umicom-studio", "c") == UMI_STATUS_OK);
    assert(umi_developer_universal_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.project_count == 1U);
    umi_studio_developer_workbench_destroy(workbench);
    return 0;
}
