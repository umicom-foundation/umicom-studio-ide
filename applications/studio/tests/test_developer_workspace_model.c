/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_workspace_model.c
 *
 * PURPOSE:
 *   Verify Project Centre composition of Batch 34 multi-root membership,
 *   deterministic active selection and reference-ordered project builds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/studio/project_centre.h"

static int add_project(UmiProjectWorkspace *workspace, const char *id,
                       const char *path)
{
    UmiProjectDescriptorSnapshot project = {0};
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, id);
    strcpy(project.name, id);
    strcpy(project.root_uri, path);
    project.enabled = 1;
    return umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace), &project) == UMI_STATUS_OK
        ? 0 : 1;
}

int main(void)
{
    UmiStudioProjectCentre *centre = NULL;
    UmiProjectWorkspace *workspace;
    UmiProjectWorkspaceRootSnapshot root = {0};
    UmiProjectWorkspaceGroupSnapshot group = {0};
    UmiProjectWorkspaceMemberSnapshot member = {0};
    UmiProjectReferenceSnapshot reference = {0};
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiStudioProjectCentreSnapshot snapshot;

    if (umi_studio_project_centre_create(&centre) != UMI_STATUS_OK) return 1;
    workspace = umi_studio_project_centre_service(centre);
    if (add_project(workspace, "framework", "/src/framework") ||
        add_project(workspace, "studio", "/src/studio")) return 2;

    root.struct_size = (uint32_t)sizeof(root);
    root.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(root.id, "source");
    strcpy(root.path, "/src");
    strcpy(root.label, "Source");
    root.enabled = 1;
    if (umi_studio_project_centre_upsert_workspace_root(centre, &root) !=
        UMI_STATUS_OK) return 3;
    group.struct_size = (uint32_t)sizeof(group);
    group.api_version = UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    strcpy(group.id, "products");
    strcpy(group.name, "Products");
    group.enabled = 1;
    if (umi_studio_project_centre_upsert_project_group(centre, &group) !=
        UMI_STATUS_OK) return 4;

    member.struct_size = (uint32_t)sizeof(member);
    member.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(member.id, "member.framework");
    strcpy(member.root_id, "source");
    strcpy(member.project_id, "framework");
    strcpy(member.group_id, "products");
    member.order = 20;
    member.enabled = 1;
    if (umi_studio_project_centre_upsert_project_member(centre, &member) !=
        UMI_STATUS_OK) return 5;
    strcpy(member.id, "member.studio");
    strcpy(member.project_id, "studio");
    member.order = 10;
    if (umi_studio_project_centre_upsert_project_member(centre, &member) !=
        UMI_STATUS_OK) return 6;

    reference.struct_size = (uint32_t)sizeof(reference);
    reference.api_version = UMI_PROJECT_REFERENCE_API_VERSION;
    strcpy(reference.id, "studio.framework");
    strcpy(reference.project_id, "studio");
    strcpy(reference.target_project_id, "framework");
    strcpy(reference.kind, "build");
    reference.required = 1;
    reference.available = 1;
    if (umi_project_reference_registry_upsert(
            umi_project_workspace_reference(workspace), &reference) !=
        UMI_STATUS_OK) return 7;
    if (umi_studio_project_centre_select_project(
            centre, "studio", &selection) != UMI_STATUS_OK ||
        strcmp(selection.project.id, "studio") != 0) return 8;
    if (umi_studio_project_centre_snapshot(centre, &snapshot) != UMI_STATUS_OK)
        return 9;
    if (!snapshot.has_workspace_model || !snapshot.has_build_order ||
        snapshot.build_order_status != UMI_STATUS_OK ||
        snapshot.workspace_model.root_count != 1U ||
        snapshot.workspace_model.member_count != 2U ||
        strcmp(snapshot.workspace_model.active_project_id, "studio") != 0 ||
        snapshot.build_order.project_count != 2U ||
        strcmp(snapshot.build_order.project_ids[0], "framework") != 0 ||
        strcmp(snapshot.build_order.project_ids[1], "studio") != 0) return 10;

    umi_studio_project_centre_destroy(centre);
    return 0;
}
