/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_source_control_centre.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/studio/source_control_centre.h"
int main(void){UmiStudioSourceControlCentre*p=NULL;UmiStudioSourceControlCentreSnapshot s;if(umi_studio_source_control_centre_create(&p)!=UMI_STATUS_OK)return 1;if(umi_studio_source_control_centre_snapshot(p,&s)!=UMI_STATUS_OK||!s.available)return 2;umi_studio_source_control_centre_destroy(p);return 0;}
