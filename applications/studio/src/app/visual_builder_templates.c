/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/visual_builder_templates.c
 *
 * PURPOSE:
 *   Coordinate Studio visual-builder templates through the Framework API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/visual_builder_templates.h"

#include <stdlib.h>

UmiStatus umi_studio_visual_builder_template_find(
    UmiStudioVisualBuilderCentre *centre,
    const char *template_id,
    UmiDesignerTemplate *out_template)
{
    UmiDesignerBuilderSession *session =
        umi_studio_visual_builder_centre_session(centre);
    return session != NULL
        ? umi_designer_template_registry_find(
              umi_designer_builder_session_templates(session),
              template_id,
              out_template)
        : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus umi_studio_visual_builder_template_apply(
    UmiStudioVisualBuilderCentre *centre,
    const char *template_id,
    const char *parent_id,
    const char *id_prefix)
{
    UmiDesignerBuilderSession *session =
        umi_studio_visual_builder_centre_session(centre);
    UmiDesignerTemplate *item;
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    item = (UmiDesignerTemplate *)malloc(sizeof(*item));
    if (item == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_template_registry_find(
        umi_designer_builder_session_templates(session),
        template_id,
        item);
    if (status == UMI_STATUS_OK) {
        status = umi_designer_template_instantiate(
            item,
            umi_designer_builder_session_transactions(session),
            parent_id,
            id_prefix);
    }
    free(item);
    return status;
}

size_t umi_studio_visual_builder_template_count(
    UmiStudioVisualBuilderCentre *centre)
{
    UmiDesignerBuilderSession *session =
        umi_studio_visual_builder_centre_session(centre);
    return session != NULL
        ? umi_designer_template_registry_count(
              umi_designer_builder_session_templates(session))
        : 0U;
}
