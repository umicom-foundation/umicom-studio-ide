/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_designer_history.c
 *
 * PURPOSE:
 *   Verify Studio exposes the Framework undo/redo workflow for designer edits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This test exercises the public contract rather than relying on private state,
 * making the behaviour useful as an executable example for new developers.
 */
#include "umicom/studio/declarative.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/designer_catalogue.h"
#include "umicom/studio/designer_preview.h"
#include "umicom/studio/new_project.h"
#include "umicom/studio/designer_state.h"
#include "umicom/studio/designer_project.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void){UmiStudioDeclarative *decl=NULL;UmiStudioDesigner *designer=NULL;UmiStudioDesignerSnapshot snapshot;assert(umi_studio_declarative_create(&decl)==UMI_STATUS_OK);assert(umi_studio_designer_create(decl,&designer)==UMI_STATUS_OK);assert(umi_studio_designer_add_component(designer,"label","label","root")==UMI_STATUS_OK);assert(umi_studio_designer_undo(designer)==UMI_STATUS_OK);assert(umi_studio_designer_redo(designer)==UMI_STATUS_OK);assert(umi_studio_designer_snapshot(designer,&snapshot)==UMI_STATUS_OK);assert(snapshot.undo_count==1U&&snapshot.document.component_count==2U);umi_studio_designer_destroy(designer);umi_studio_declarative_destroy(decl);return EXIT_SUCCESS;}
