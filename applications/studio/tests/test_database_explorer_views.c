/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/studio/database_explorer_views.h"
int main(void){UmiStudioServices *services;UmiStudioDatabaseExplorerCentre *c=NULL;UmiStudioDatabaseView v;assert(umi_studio_services_create(NULL,NULL,&services)==UMI_STATUS_OK);assert(umi_studio_database_explorer_create(services,&c)==UMI_STATUS_OK);assert(umi_studio_database_view_build(c,UMI_STUDIO_DATABASE_VIEW_SCHEMA,&v)==UMI_STATUS_OK);assert(strcmp(v.id,"schema")==0&&v.row_count==1U&&strstr(v.summary,"memory")!=NULL);umi_studio_database_explorer_destroy(c);umi_studio_services_destroy(services);return 0;}
