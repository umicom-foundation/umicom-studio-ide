/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/editor_workspace.c
 *
 * PURPOSE:
 *   Implement Studio editor-workspace composition over the reusable Framework editor session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#include "umicom/studio/editor_workspace.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioEditorWorkspace{UmiStudioServices*services;UmiEditorSession*editor;uint64_t revision;};
UmiStatus umi_studio_editor_workspace_create(UmiStudioServices*services,UmiStudioEditorWorkspace**out){UmiStudioEditorWorkspace*w;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;w=calloc(1U,sizeof(*w));if(w==NULL)return UMI_STATUS_OUT_OF_MEMORY;w->services=services;w->revision=1U;s=umi_editor_session_create(&w->editor);if(s!=UMI_STATUS_OK){free(w);return s;}*out=w;return UMI_STATUS_OK;}
void umi_studio_editor_workspace_destroy(UmiStudioEditorWorkspace*w){if(w==NULL)return;umi_editor_session_destroy(w->editor);free(w);}
UmiStatus umi_studio_editor_workspace_snapshot(UmiStudioEditorWorkspace*w,UmiStudioEditorWorkspaceSnapshot*o){if(w==NULL||o==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(o,0,sizeof(*o));o->struct_size=(uint32_t)sizeof(*o);o->api_version=1U;o->revision=w->revision;o->available=1;return umi_editor_session_snapshot(w->editor,&o->editor);}
UmiEditorSession *umi_studio_editor_workspace_session(UmiStudioEditorWorkspace*w){return w!=NULL?w->editor:NULL;}
