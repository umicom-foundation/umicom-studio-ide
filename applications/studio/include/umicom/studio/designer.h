/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/designer.h
 *
 * PURPOSE:
 *   Compose the reusable Framework designer engine into Studio authoring operations, undo/redo, selection and preview.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio-facing contract composes reusable Framework capabilities.
 * Generic designer and declarative logic remains owned by Umicom Framework.
 */

#ifndef UMICOM_STUDIO_DESIGNER_H
#define UMICOM_STUDIO_DESIGNER_H
#include "umicom/studio/declarative.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStudioDesigner UmiStudioDesigner;
typedef struct UmiStudioDesignerSnapshot { UmiDesignerDocumentSnapshot document; size_t palette_items; size_t selected_items; size_t undo_count; size_t redo_count; } UmiStudioDesignerSnapshot;
UmiStatus umi_studio_designer_create(UmiStudioDeclarative *declarative,UmiStudioDesigner **out_designer);
void umi_studio_designer_destroy(UmiStudioDesigner *designer);
UmiStatus umi_studio_designer_bind_commands(UmiStudioDesigner *designer,UmiCommandRegistry *commands);
UmiStatus umi_studio_designer_new(UmiStudioDesigner *designer,const char *application_id);
UmiStatus umi_studio_designer_load_text(UmiStudioDesigner *designer,const char *source,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_designer_add_component(UmiStudioDesigner *designer,const char *node_id,const char *component_type,const char *parent_id);
UmiStatus umi_studio_designer_set_property(UmiStudioDesigner *designer,const char *node_id,const char *property_name,const char *value_text);
UmiStatus umi_studio_designer_select(UmiStudioDesigner *designer,const char *node_id);
UmiStatus umi_studio_designer_undo(UmiStudioDesigner *designer);
UmiStatus umi_studio_designer_redo(UmiStudioDesigner *designer);
UmiStatus umi_studio_designer_preview(UmiStudioDesigner *designer,char *out_text,size_t capacity,UmiDeclDiagnosticList *diagnostics);
UmiStatus umi_studio_designer_snapshot(const UmiStudioDesigner *designer,UmiStudioDesignerSnapshot *out_snapshot);
UmiDesignerDocument *umi_studio_designer_document(UmiStudioDesigner *designer);
#ifdef __cplusplus
}
#endif
#endif
