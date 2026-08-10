/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/designer.c
 *
 * PURPOSE:
 *   Implement Studio authoring operations over the Framework designer engine while keeping semantic state outside GTK widgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio adapter keeps product composition thin: it delegates reusable
 * behaviour to Framework services and adds only Studio-specific coordination.
 */

#include "umicom/studio/designer.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioDesigner{UmiStudioDeclarative *declarative;UmiDesignerDocument *document;UmiDesignerHistory *history;UmiDesignerSelection selection;};
static void release_document(UmiStudioDesigner *d){umi_designer_history_destroy(d->history);d->history=NULL;umi_designer_document_destroy(d->document);d->document=NULL;umi_designer_selection_clear(&d->selection);}
static UmiStatus attach_document(UmiStudioDesigner *d,UmiDesignerDocument *doc){UmiStatus s;release_document(d);d->document=doc;s=umi_designer_history_create(doc,&d->history);if(s!=UMI_STATUS_OK){umi_designer_document_destroy(doc);d->document=NULL;}return s;}
UmiStatus umi_studio_designer_create(UmiStudioDeclarative *decl,UmiStudioDesigner **out){UmiStudioDesigner *d;UmiStatus s;if(decl==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;d=calloc(1U,sizeof(*d));if(d==NULL)return UMI_STATUS_OUT_OF_MEMORY;d->declarative=decl;s=umi_studio_designer_new(d,"org.umicom.studio.designer");if(s!=UMI_STATUS_OK){free(d);return s;}*out=d;return UMI_STATUS_OK;}
void umi_studio_designer_destroy(UmiStudioDesigner *d){if(d==NULL)return;release_document(d);free(d);}
UmiStatus umi_studio_designer_bind_commands(UmiStudioDesigner *d,UmiCommandRegistry *commands){return d!=NULL?umi_designer_register_commands(commands,d->history):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_designer_new(UmiStudioDesigner *d,const char *app){UmiDesignerDocument *doc=NULL;UmiDeclNode root;UmiStatus s;if(d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_designer_document_create(app,&doc);if(s==UMI_STATUS_OK)s=umi_decl_node_init(&root,"root","window","-");if(s==UMI_STATUS_OK)s=umi_decl_document_add_node(umi_designer_document_declarative(doc),&root);if(s!=UMI_STATUS_OK){umi_designer_document_destroy(doc);return s;}umi_designer_document_mark_saved(doc);return attach_document(d,doc);}
UmiStatus umi_studio_designer_load_text(UmiStudioDesigner *d,const char *src,UmiDeclDiagnosticList *diags){UmiDeclDocument *decl=NULL;UmiDesignerDocument *doc=NULL;UmiStatus s;if(d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_studio_declarative_parse(d->declarative,src,&decl,diags);if(s==UMI_STATUS_OK)s=umi_designer_document_from_declarative(decl,1,&doc);if(s!=UMI_STATUS_OK){umi_decl_document_destroy(decl);return s;}umi_designer_document_mark_saved(doc);return attach_document(d,doc);}
UmiStatus umi_studio_designer_add_component(UmiStudioDesigner *d,const char *id,const char *type,const char *parent){UmiDeclNode node;UmiDesignerOperation op;UmiStatus s;if(d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_node_init(&node,id,type,parent);if(s==UMI_STATUS_OK)s=umi_designer_operation_add(&node,&op);if(s==UMI_STATUS_OK)s=umi_designer_history_execute(d->history,&op);return s;}
UmiStatus umi_studio_designer_set_property(UmiStudioDesigner *d,const char *id,const char *name,const char *value){UmiDeclNode before,after;UmiDesignerOperation op;UmiStatus s;if(d==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_decl_document_find_node(umi_designer_document_declarative(d->document),id,&before);after=before;if(s==UMI_STATUS_OK)s=umi_decl_node_set_attribute(&after,name,UMI_DECL_VALUE_STRING,value);if(s==UMI_STATUS_OK)s=umi_designer_operation_set_property(&before,&after,name,&op);if(s==UMI_STATUS_OK)s=umi_designer_history_execute(d->history,&op);return s;}
UmiStatus umi_studio_designer_select(UmiStudioDesigner *d,const char *id){return d!=NULL?umi_designer_selection_set_primary(&d->selection,id):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_designer_undo(UmiStudioDesigner *d){return d!=NULL?umi_designer_history_undo(d->history):UMI_STATUS_INVALID_ARGUMENT;}UmiStatus umi_studio_designer_redo(UmiStudioDesigner *d){return d!=NULL?umi_designer_history_redo(d->history):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_designer_preview(UmiStudioDesigner *d,char *out,size_t cap,UmiDeclDiagnosticList *diags){return d!=NULL?umi_designer_preview(d->document,umi_studio_declarative_schema(d->declarative),out,cap,diags):UMI_STATUS_INVALID_ARGUMENT;}
UmiStatus umi_studio_designer_snapshot(const UmiStudioDesigner *d,UmiStudioDesignerSnapshot *out){UmiDesignerPalette palette;UmiStatus s;if(d==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));s=umi_designer_document_snapshot(d->document,&out->document);if(s==UMI_STATUS_OK)s=umi_designer_palette_build(umi_studio_declarative_components(d->declarative),NULL,&palette);if(s==UMI_STATUS_OK){out->palette_items=palette.count;out->selected_items=d->selection.count;out->undo_count=umi_designer_history_undo_count(d->history);out->redo_count=umi_designer_history_redo_count(d->history);umi_designer_palette_dispose(&palette);}return s;}
UmiDesignerDocument *umi_studio_designer_document(UmiStudioDesigner *d){return d!=NULL?d->document:NULL;}
