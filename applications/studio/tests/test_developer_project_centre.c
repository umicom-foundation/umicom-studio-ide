/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_project_centre.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/studio/project_centre.h"
int main(void){UmiStudioProjectCentre*p=NULL;UmiStudioProjectCentreSnapshot s;if(umi_studio_project_centre_create(&p)!=UMI_STATUS_OK)return 1;if(umi_studio_project_centre_snapshot(p,&s)!=UMI_STATUS_OK||!s.available)return 2;umi_studio_project_centre_destroy(p);return 0;}
