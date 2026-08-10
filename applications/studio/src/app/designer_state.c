/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/designer_state.c
 *
 * PURPOSE:
 *   Produce a text report for Studio tools and diagnostics without exposing private designer structures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio adapter keeps product composition thin: it delegates reusable
 * behaviour to Framework services and adds only Studio-specific coordination.
 */

#include "umicom/studio/designer_state.h"
#include <stdio.h>
UmiStatus umi_studio_designer_state_report(const UmiStudioDesigner *d,char *out,size_t cap){UmiStudioDesignerSnapshot s;int n;UmiStatus status=umi_studio_designer_snapshot(d,&s);if(status!=UMI_STATUS_OK)return status;if(out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out,cap,"Application: %s\nComponents: %zu\nPalette items: %zu\nSelected: %zu\nUndo: %zu\nRedo: %zu\nRevision: %llu\nDirty: %s\n",s.document.application_id,s.document.component_count,s.palette_items,s.selected_items,s.undo_count,s.redo_count,(unsigned long long)s.document.revision,s.document.dirty?"yes":"no");return n>=0&&(size_t)n<cap?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
