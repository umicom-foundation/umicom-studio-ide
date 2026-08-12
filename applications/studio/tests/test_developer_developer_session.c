/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_developer_session.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/studio/developer_session.h"
int main(void){UmiStudioDeveloperSession*p=NULL;UmiStudioDeveloperSessionSnapshot s;if(umi_studio_developer_session_create(&p)!=UMI_STATUS_OK)return 1;if(umi_studio_developer_session_snapshot(p,&s)!=UMI_STATUS_OK||s.revision==0U)return 2;umi_studio_developer_session_destroy(p);return 0;}
