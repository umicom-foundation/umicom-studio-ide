/* Umicom Studio IDE | Security Centre views v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_views.h"
#include <stdio.h>
#include <string.h>

const char *umi_studio_security_view_id(UmiStudioSecurityViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_SECURITY_VIEW_OVERVIEW: return "overview";
        case UMI_STUDIO_SECURITY_VIEW_IDENTITIES: return "identities";
        case UMI_STUDIO_SECURITY_VIEW_ROLES: return "roles";
        case UMI_STUDIO_SECURITY_VIEW_WORKSPACE_TRUST: return "workspace-trust";
        case UMI_STUDIO_SECURITY_VIEW_SECRETS: return "secrets";
        case UMI_STUDIO_SECURITY_VIEW_APPROVALS: return "approvals";
        case UMI_STUDIO_SECURITY_VIEW_PACKAGE_TRUST: return "package-trust";
        case UMI_STUDIO_SECURITY_VIEW_AUDIT: return "audit";
        default: return NULL;
    }
}

static const char *view_title(UmiStudioSecurityViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_SECURITY_VIEW_OVERVIEW: return "Security Overview";
        case UMI_STUDIO_SECURITY_VIEW_IDENTITIES: return "Identities";
        case UMI_STUDIO_SECURITY_VIEW_ROLES: return "Roles and Permissions";
        case UMI_STUDIO_SECURITY_VIEW_WORKSPACE_TRUST: return "Workspace Trust";
        case UMI_STUDIO_SECURITY_VIEW_SECRETS: return "Secret References";
        case UMI_STUDIO_SECURITY_VIEW_APPROVALS: return "Audited Approvals";
        case UMI_STUDIO_SECURITY_VIEW_PACKAGE_TRUST: return "Package Trust";
        case UMI_STUDIO_SECURITY_VIEW_AUDIT: return "Security Audit";
        default: return NULL;
    }
}

static size_t view_items(const UmiStudioSecurityCentreSnapshot *snapshot,
                         UmiStudioSecurityViewKind kind)
{
    switch (kind) {
        case UMI_STUDIO_SECURITY_VIEW_OVERVIEW: return 8U;
        case UMI_STUDIO_SECURITY_VIEW_IDENTITIES: return snapshot->governance.identities;
        case UMI_STUDIO_SECURITY_VIEW_ROLES: return snapshot->governance.roles;
        case UMI_STUDIO_SECURITY_VIEW_WORKSPACE_TRUST: return snapshot->governance.assignments;
        case UMI_STUDIO_SECURITY_VIEW_SECRETS: return snapshot->governance.secret_references;
        case UMI_STUDIO_SECURITY_VIEW_APPROVALS: return snapshot->governance.approvals;
        case UMI_STUDIO_SECURITY_VIEW_PACKAGE_TRUST: return snapshot->governance.trusted_signers;
        case UMI_STUDIO_SECURITY_VIEW_AUDIT: return snapshot->governance.audit_entries;
        default: return 0U;
    }
}

UmiStatus umi_studio_security_view_build(const UmiStudioSecurityCentre *centre,
                                         uint64_t now_ns,
                                         UmiStudioSecurityViewKind kind,
                                         UmiStudioSecurityView *out_view)
{
    UmiStudioSecurityCentreSnapshot snapshot;
    const char *id = umi_studio_security_view_id(kind);
    const char *title = view_title(kind);
    int first;
    int second;
    int third;
    if (centre == NULL || out_view == NULL || id == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_studio_security_centre_snapshot(centre,now_ns,&snapshot) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    (void)memset(out_view,0,sizeof(*out_view));
    out_view->kind = kind;
    out_view->item_count = view_items(&snapshot,kind);
    out_view->revision = snapshot.revision;
    first = snprintf(out_view->id,sizeof(out_view->id),"%s",id);
    second = snprintf(out_view->title,sizeof(out_view->title),"%s",title);
    third = snprintf(out_view->summary,sizeof(out_view->summary),
                     "%zu items; %zu denied events; %zu pending approvals; %zu rotations due",
                     out_view->item_count,
                     snapshot.governance.denied_entries,
                     snapshot.pending_approvals,
                     snapshot.secrets_due_rotation);
    if (first < 0 || second < 0 || third < 0 ||
        (size_t)first >= sizeof(out_view->id) ||
        (size_t)second >= sizeof(out_view->title) ||
        (size_t)third >= sizeof(out_view->summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
