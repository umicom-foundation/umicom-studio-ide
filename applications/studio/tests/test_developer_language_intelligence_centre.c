/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_developer_language_intelligence_centre.c
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include "umicom/studio/language_intelligence_centre.h"
int main(void){UmiStudioLanguageIntelligenceCentre*p=NULL;UmiStudioLanguageIntelligenceCentreSnapshot s;if(umi_studio_language_intelligence_centre_create(&p)!=UMI_STATUS_OK)return 1;if(umi_studio_language_intelligence_centre_snapshot(p,&s)!=UMI_STATUS_OK||!s.available)return 2;umi_studio_language_intelligence_centre_destroy(p);return 0;}
