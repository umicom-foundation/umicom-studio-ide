/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_workspace_inheritance.c
 *
 * PURPOSE:
 *   Verify Project Centre composes Framework-owned group/project settings and
 *   variables without duplicating inheritance logic in the Studio product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/studio/project_centre.h"

int main(void)
{
    UmiStudioProjectCentre *centre = NULL;
    UmiProjectWorkspace *workspace;
    UmiProjectWorkspaceModel *model;
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectWorkspaceRootSnapshot root = {0};
    UmiProjectWorkspaceGroupSnapshot group = {0};
    UmiProjectWorkspaceMemberSnapshot member = {0};
    UmiProjectWorkspaceSettingSnapshot setting = {0};
    UmiProjectWorkspaceResolvedSetting resolved;
    UmiProjectVariableSnapshot variable = {0};

    if (umi_studio_project_centre_create(&centre) != UMI_STATUS_OK) return 1;
    workspace = umi_studio_project_centre_service(centre);
    model = umi_studio_project_centre_workspace_model(centre);
    if (workspace == NULL || model == NULL) return 2;
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, "studio");
    strcpy(project.name, "Studio");
    strcpy(project.root_uri, "/src/studio");
    project.enabled = 1;
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace), &project) !=
        UMI_STATUS_OK) return 3;
    root.struct_size = (uint32_t)sizeof(root);
    root.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(root.id, "source");
    strcpy(root.path, "/src");
    root.enabled = 1;
    if (umi_studio_project_centre_upsert_workspace_root(centre, &root) !=
        UMI_STATUS_OK) return 4;
    group.struct_size = (uint32_t)sizeof(group);
    group.api_version = UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    strcpy(group.id, "applications");
    strcpy(group.name, "Applications");
    group.enabled = 1;
    if (umi_studio_project_centre_upsert_project_group(centre, &group) !=
        UMI_STATUS_OK) return 5;
    member.struct_size = (uint32_t)sizeof(member);
    member.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(member.id, "member.studio");
    strcpy(member.root_id, "source");
    strcpy(member.project_id, "studio");
    strcpy(member.group_id, "applications");
    member.enabled = 1;
    if (umi_studio_project_centre_upsert_project_member(centre, &member) !=
        UMI_STATUS_OK) return 6;

    setting.struct_size = (uint32_t)sizeof(setting);
    setting.api_version = UMI_PROJECT_WORKSPACE_SETTING_API_VERSION;
    strcpy(setting.id, "workspace.generator");
    strcpy(setting.key, "generator");
    strcpy(setting.value, "Ninja");
    setting.scope = UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE;
    if (umi_studio_project_centre_upsert_workspace_setting(
            centre, &setting) != UMI_STATUS_OK) return 7;
    strcpy(setting.id, "group.generator");
    strcpy(setting.owner_id, "applications");
    strcpy(setting.value, "Ninja Multi-Config");
    setting.scope = UMI_PROJECT_WORKSPACE_SETTING_GROUP;
    if (umi_studio_project_centre_upsert_workspace_setting(
            centre, &setting) != UMI_STATUS_OK) return 8;
    if (umi_project_workspace_model_resolve_setting(
            model, "studio", "generator", &resolved) != UMI_STATUS_OK ||
        strcmp(resolved.value, "Ninja Multi-Config") != 0 ||
        resolved.source_scope != UMI_PROJECT_WORKSPACE_SETTING_GROUP) return 9;

    variable.struct_size = (uint32_t)sizeof(variable);
    variable.api_version = UMI_PROJECT_VARIABLE_API_VERSION;
    strcpy(variable.id, "workspace.jobs");
    strcpy(variable.name, "jobs");
    strcpy(variable.value, "2");
    if (umi_project_variable_registry_upsert(
            umi_project_workspace_variable(workspace), &variable) !=
        UMI_STATUS_OK) return 10;
    strcpy(variable.id, "studio.jobs");
    strcpy(variable.project_id, "studio");
    strcpy(variable.value, "8");
    if (umi_project_variable_registry_upsert(
            umi_project_workspace_variable(workspace), &variable) !=
        UMI_STATUS_OK) return 11;
    if (umi_project_workspace_model_resolve_variable(
            model, "studio", "jobs", &resolved) != UMI_STATUS_OK ||
        strcmp(resolved.value, "8") != 0) return 12;

    umi_studio_project_centre_destroy(centre);
    return 0;
}
