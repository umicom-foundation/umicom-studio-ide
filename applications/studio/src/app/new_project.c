/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/new_project.c
 *
 * PURPOSE:
 *   Generate small semantic application templates that the visual designer can open and extend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio adapter keeps product composition thin: it delegates reusable
 * behaviour to Framework services and adds only Studio-specific coordination.
 */

#include "umicom/studio/new_project.h"
#include <stdio.h>
UmiStatus umi_studio_new_project_template(UmiStudioProjectTemplate kind,const char *id,char *out,size_t cap){int n;if(!umi_decl_id_is_valid(id)||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;if(kind==UMI_STUDIO_TEMPLATE_EDITOR)n=snprintf(out,cap,"application %s\nversion 1.0.0\ncomponent root window -\nproperty root title %s\ncomponent split split root\ncomponent explorer tree split\ncomponent editor editor split\n",id,id);else if(kind==UMI_STUDIO_TEMPLATE_DASHBOARD)n=snprintf(out,cap,"application %s\nversion 1.0.0\ncomponent root window -\nproperty root title %s\ncomponent tabs tabs root\ncomponent chart chart tabs\ncomponent table table tabs\n",id,id);else n=snprintf(out,cap,"application %s\nversion 1.0.0\ncomponent root window -\nproperty root title %s\ncomponent content pane root\n",id,id);return n>=0&&(size_t)n<cap?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED;}
