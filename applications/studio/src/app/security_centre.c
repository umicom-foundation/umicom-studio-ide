/* Umicom Studio IDE | Security Centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/security_centre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
UmiStatus umi_studio_security_centre_create(UmiStudioServices *services,UmiStudioSecurityCentre **out_centre)
{
    UmiStudioSecurityCentre *centre;
    if (services == NULL || out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiStudioSecurityCentre *)calloc(1U,sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->services = services; umi_security_governance_centre_init(&centre->governance);
    (void)snprintf(centre->active_view,sizeof(centre->active_view),"overview"); centre->revision = 1U; *out_centre = centre;
    return UMI_STATUS_OK;
}
void umi_studio_security_centre_destroy(UmiStudioSecurityCentre *centre) { free(centre); }
UmiStatus umi_studio_security_centre_activate(UmiStudioSecurityCentre *centre,const char *view_id)
{
    int length;
    if (centre == NULL || view_id == NULL || view_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = snprintf(centre->active_view,sizeof(centre->active_view),"%s",view_id);
    if (length < 0 || (size_t)length >= sizeof(centre->active_view)) return UMI_STATUS_CAPACITY_EXCEEDED;
    centre->revision += 1U; return UMI_STATUS_OK;
}
UmiStatus umi_studio_security_centre_snapshot(const UmiStudioSecurityCentre *centre,uint64_t now_ns,UmiStudioSecurityCentreSnapshot *out_snapshot)
{
    size_t index;
    if (centre == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    umi_security_governance_snapshot(&centre->governance,&out_snapshot->governance);
    for (index = 0U; index < centre->governance.identities.count; ++index) if (centre->governance.identities.items[index].enabled) out_snapshot->enabled_identities += 1U;
    for (index = 0U; index < centre->governance.approvals.count; ++index) if (centre->governance.approvals.items[index].state == UMI_SECURITY_APPROVAL_PENDING) out_snapshot->pending_approvals += 1U;
    for (index = 0U; index < centre->governance.secrets.count; ++index) if (umi_security_secret_rotation_due(&centre->governance.secrets.items[index],now_ns)) out_snapshot->secrets_due_rotation += 1U;
    (void)snprintf(out_snapshot->active_view,sizeof(out_snapshot->active_view),"%s",centre->active_view); out_snapshot->revision = centre->revision;
    return UMI_STATUS_OK;
}
