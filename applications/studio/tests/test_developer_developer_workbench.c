/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_developer_workbench.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/studio/developer_workbench.h"
int main(void){UmiStudioDeveloperWorkbench*p=NULL;UmiStudioDeveloperWorkbenchSnapshot s;if(umi_studio_developer_workbench_create(&p)!=UMI_STATUS_OK)return 1;if(umi_studio_developer_workbench_snapshot(p,&s)!=UMI_STATUS_OK||!s.available)return 2;umi_studio_developer_workbench_destroy(p);return 0;}
